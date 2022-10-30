#include "preproc/preprocess.hpp"

#include "string_functions.hpp"

namespace fs = std::filesystem;

/// Dumps the given file line by line to 'output'.
/// Returns false if there was an error when opening the file and true otherwise.
static bool read_file(const fs::path &filename, std::vector<Line> &output) {
	std::ifstream file{filename};

	if (!file.is_open()) return false;

	std::string line;
	size_t num = 1;

	while (std::getline(file, line)) {
		output.push_back(Line(line, filename, num));
		num++;
	}

	return true;
}

static void preprocess_includes(std::vector<Line> &output) {
	// Include directives
	std::unordered_set<std::string> includedFiles;

	// DONE: Heriarchical view of inclusion for error logging (sort of)

	auto it = output.begin();
	while (it != output.end()) {
		Line &line = *it;

		if (line.content[0] == '#') {
			// Chech for preprocessor directives

			if (line.content.find("#include") != std::string::npos) {
				// TODO: Make marking files more robust
				fs::path includeFile = line.content.substr(line.content.find("\"") + 1,
				                                           line.content.rfind("\"") - line.content.find("\"") - 1);

				includeFile = line.file.parent_path() / includeFile;
				includeFile = includeFile.lexically_normal();

				if (line.content.find("#include_recursive") == std::string::npos &&
				    includedFiles.find(includeFile) !=
				        includedFiles.end())  // If file already included and not recursive
				{
					// Already included
					it = output.erase(it);
					continue;
				}

				includedFiles.insert(includeFile);

				std::vector<Line> include_output;

				if (!read_file(includeFile, include_output)) {
					compile_error(line, fmt::format("Included file does not exists: {}", includeFile));
				}

				it = output.erase(it);

				// NOTE: Bug was here because insert might render iterator invalid
				it = output.insert(it, include_output.begin(), include_output.end());

				continue;
			}
		}

		it++;
	}
}

static void trim_comments(Line &line) {
	iterate_ignore_quotes(line.content, [](std::string &content, size_t &i) {
		if (content[i] == ';') {
			content.erase(i);  // from i to end of string
			return 1;
		}

		return 0;
	});
}

static void trim_whitespace(Line &line) {
	// Remove blanks at the beginning
	size_t i = 0;
	while (true) {
		if (!isblank(line.content[i])) break;

		i++;
	}

	if (i > 0) {
		line.content.erase(0, i);
	}

	// Remove blanks between arguments
	iterate_ignore_quotes(line.content, [](std::string &content, size_t &i) {
		// Remove this space if previous character is ',' or '('
		if (isblank(content[i]) && (strchr(",(", content[i - 1]) || isblank(content[i - 1]))) {
			content.erase(i, 1);
			i--;
		}

		// Remove this space if next character is ',' or ')'
		if (isblank(content[i]) && i < content.length() - 1 && strchr(",)", content[i + 1])) {
			content.erase(i, 1);
			i--;
		}

		return 0;
	});

	// Remove all blanks at the end
	// Useful for instructions with no arguments
	i = line.content.length() - 1;
	while (i > 0 && i < line.content.length())  // Avoid integer overflow if length is 0
	{
		if (!isblank(line.content[i])) break;

		i--;
	}

	if (i > 0 && i < line.content.length() - 1) {
		line.content.erase(i + 1);
	}
}

static void get_macro_definition(std::vector<Line> &output, size_t &idx, MacroMap &macros) {
	Line &line = output[idx];

	size_t pos;
	if ((pos = line.content.find("#define")) != std::string::npos) {
		pos += sizeof("#define");  // size of "#define" *with* NUL character, which counts as the space

		size_t posValue = line.content.find(' ', pos);

		// #define MACRO VALUE
		//         ^pos ^posValue

		std::string name = line.content.substr(pos, posValue - pos);

		macros[name].push_back(line.content.substr(posValue + 1));

		// std::cout << "Defined: \"" << name << "\"\n";

		line.content.clear();  // transform directive into empty line
	} else if ((pos = line.content.find("#macro ")) !=
	           std::string::npos)  // NOTE: Space is required to not check against #macrogroup
	{
		pos += sizeof("#macro");

		std::string name = line.content.substr(pos);

		//#macro MACRO
		//  ..
		//#endmacro

		size_t startOfMacro = idx;

		while (true) {
			if (idx == output.size() - 1)
				compile_error(output[startOfMacro], fmt::format("Unfinished macro: {}", name));

			idx++;
			// line = output[idx];  NOTE: Fuck you reference re-assignment
			Line &currentLine = output[idx];

			if (line.content[0] == '#' &&
			    find_delimited_string(currentLine.content, "#endmacro", "") != std::string::npos)
				break;

			macros[name].push_back(currentLine.content);
		}

		if (macros[name].size() == 0) {
			compile_error(output[idx], fmt::format("Empty macro: {}", name));  // DONE: Line numbers
		} else if (macros[name].size() == 1)
			compile_warning(line,
			                fmt::format("Singleline macro {} defined with #macro directive could use #define", name));

		for (size_t i = startOfMacro; i <= idx; i++) {
			output[i].content.clear();
		}
	}
}

static void process_macro_group(std::vector<Line> &output, size_t &idx, std::stack<MacroMap> &macrogroups,
                                size_t &macrogroup_index) {
	Line &line = output[idx];

	if (line.content.find("#macrogroup") != std::string::npos) {
		macrogroups.push(MacroMap());

		MacroMap &current = macrogroups.top();
		current["GROUP_INDEX"].push_back(std::to_string(++macrogroup_index));
		current["GROUP_ID"].push_back(random_string(16));

		line.content.clear();
	} else if (line.content.find("#endmacrogroup") != std::string::npos) {
		if (macrogroups.size() == 0) compile_warning(line.line, "Non started macrogroup.");

		if (macrogroups.size() > 0) {
			macrogroups.pop();

			line.content.clear();
		}
	} else if (macrogroups.size() > 0) {
		auto &top = macrogroups.top();
		search_macro(output, idx, top);
	}
}

std::vector<Line> preprocess(const fs::path filename) {
	std::vector<Line> output;

	if (!read_file(filename, output)) {
		compile_error(0,
		              fmt::format("Source file does not exists: {}", filename));  // This is checked in main but oh well
	}

	preprocess_includes(output);
	for (Line &line : output) {
		// Remove trailing spaces / comments
		trim_comments(line);
		trim_whitespace(line);
	}

	MacroMap macros;

	std::stack<MacroMap> macrogroups;
	size_t macrogroup_index = 0;

	size_t idx = 0;
	while (idx < output.size()) {
		if (output.empty()) {  // don't bother with all the checks if the line is empty
			idx++;
			continue;
		}

		// Process macros definitions
		get_macro_definition(output, idx, macros);

		// Conditions
		// #if VERSION eq 1.0.0
		//
		// #endif
		if (output[idx].content.rfind("#if", 0) == 0) {
			auto &line = output[idx].content;

			auto tokenized = string_split(line, ' ');
			auto &macro_name = tokenized[1];
			auto macro_it = macros.find(macro_name);

			enum { DEF, NOT_DEF, EQUAL, NOT_EQUAL } op;
			{
				auto &s_op = tokenized[2];
				if (s_op == "def")
					op = DEF;
				else if (s_op == "ndef")
					op = NOT_DEF;
				else if (s_op == "eq")
					op = EQUAL;
				else if (s_op == "ne")
					op = NOT_EQUAL;
				else
					compile_error(output[idx], fmt::format("uknown comparison: {}", s_op));
			}

			bool condition = false;
			// Check for a given "type" of operation and invert the result for its given contrary
			switch (op) {
			case DEF:
			case NOT_DEF:
				condition = macro_it != macros.end();
				if (op == DEF)
					fmt::print("Is macro {} defined?: {}\n", macro_name, condition ? "yes" : "no");
				else {
					condition = !condition;
					fmt::print("Is macro {} not defined?: {}\n", macro_name, condition ? "yes" : "no");
				}
				break;
			case EQUAL:
			case NOT_EQUAL:
				auto macro_value = std::string();
				if (macro_it == macros.end())
					compile_error(output[idx], fmt::format("macro {} not defined", macro_name));
				else
					macro_value = macro_it->second[0];

				auto &matching = tokenized[3];
				condition = macro_value == matching;

				if (op == EQUAL)
					fmt::print("Is macro {}({}) equal to {}?: {}\n", macro_name, macro_value, matching,
					           condition ? "yes" : "no");
				if (op == NOT_EQUAL) {
					condition = !condition;
					fmt::print("Is macro {}({}) not equal to {}?: {}\n", macro_name, macro_value, matching,
					           condition ? "yes" : "no");
				}
				break;
			}

			size_t pos = idx;
			size_t depth = 1;
			while (depth != 0) {
				pos++;
				if (pos >= output.size()) compile_error(output[idx], "unfinished #if directive");
				if (output[pos].content.rfind("#if", 0) == 0) depth++;
				if (output[pos].content.rfind("#endif", 0) == 0) depth--;
			}  // pos is now at the matching #endif's line

			if (condition) {  // remove the directives
				output[idx].content.clear();
				output[pos].content.clear();
			} else {  // remove the entire body
				for (auto i = idx; i <= pos; i++) {
					output[i].content.clear();
				}
			}
		}

		// Expand macros
		search_macro(output, idx, macros);

		// Expand macrogroup macros
		process_macro_group(output, idx, macrogroups, macrogroup_index);

		idx++;
	}

	// TODO: Dealing with single-line macros shouldn't need a wholeass vector

	// Remove empty lines
	std::vector<Line> trimmedOutput;
	trimmedOutput.reserve(output.size());

	for (auto &line : output) {
		if (!line.content.empty()) trimmedOutput.push_back(std::move(line));
	}

	return trimmedOutput;
}
