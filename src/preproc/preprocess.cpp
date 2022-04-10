#include "preproc/preprocess.hpp"

/// Dumps the given file line by line to 'output'.
/// Returns false if there was an error when opening the file and true otherwise.
static bool read_file(const char *filename, std::vector<Line> &output)
{
	std::ifstream file(filename);

	if(!file.is_open()) return false;

	std::string line;
	size_t num = 1;

	while(std::getline(file, line))
	{
		output.push_back(Line(line, filename, num));
		num++;
	}

	return true;
}

/// Iterates over a string and calls a function for every characters in it, but ignores anything wrapped in double quotes
/// @param func Function used. Non zero return value indicates to break out of loop
static void iterate_ignore_quotes(std::string &str, size_t pos, int (*func)(std::string &, size_t &))
{
	bool inQuotes = false;

	while(pos < str.length())
	{
		if(inQuotes)
		{
			if(str[pos] == '\\')
			{
				pos++;
			}
			else if(str[pos] == '\"')
			{
				inQuotes = false;
			}
		}
		else if(str[pos] == '"')
		{
			inQuotes = true;
		}
		else
		{
			int res = func(str, pos);

			if(res != 0) break;
		}

		pos++;
	} 
}

static void iterate_ignore_quotes(std::string &str, int (*func)(std::string &, size_t &))
{
	iterate_ignore_quotes(str, 0, func);
}



void preprocess(const char *filename, std::vector<Line> &output)
{
	if(!read_file(filename, output))
	{
		compile_error(0, "Source file does not exists: %s", filename); // This is checked in main but oh well
	}
	
	std::string filenameDirectory = filename;
	filenameDirectory.erase(filenameDirectory.find_last_of('/') + 1);
	
	// Include directives
	auto it = output.begin();
	std::unordered_set<std::string> includedFiles;

	// DONE: Heriarchical view of inclusion for error logging (sort of)
	
	while(it != output.end())
	{
		Line &line = *it;

		if(line.content[0] == '#')
		{
			// Chech for preprocessor directives

			if(line.content.find("#include") != std::string::npos)
			{
				// TODO: Make marking files more robust
				std::string includeFile = line.content.substr(line.content.find("\"") + 1, line.content.rfind("\"") - line.content.find("\"") - 1);
				if(line.content.find("#include_recursive") == std::string::npos && includedFiles.find(includeFile) != includedFiles.end()) // If file already included and not recursive
				{
					// Already included
					it = output.erase(it);
					continue;
				}

				includedFiles.insert(includeFile);

				includeFile = filenameDirectory + includeFile;

				std::vector<Line> include_output;

				if(!read_file(includeFile.c_str(), include_output))
				{
					compile_error(line.line, "Included file does not exists: %s", includeFile.c_str());
				}

				it = output.erase(it);

				// NOTE: Bug was here because insert might render iterator invalid
				it = output.insert(it, include_output.begin(), include_output.end());

				continue;
			}
		}

		it++;
	}
	
	// Remove trailing spaces / comments
	it = output.begin();
	while(it != output.end())
	{
		Line &line = *it;
		
		// Remove comments
		iterate_ignore_quotes(line.content,
			[](std::string &content, size_t &i)
			{
				if(content[i] == ';')
				{
					content.erase(i);
					return 1;
				}

				return 0;
			}
		);

		// Remove blanks at the beginning
		size_t i = 0;
		while(true)
		{
			if(!isblank(line.content[i])) break;

			i++;
		}

		if(i > 0)
		{
			line.content.erase(0, i);
		}

		// Remove blanks between arguments
		iterate_ignore_quotes(line.content,
			[](std::string &content, size_t &i)
			{
				// Remove this space if previous character is one of those
				if(isblank(content[i]) && (strchr(",(", content[i-1]) || isblank(content[i-1])))
				{
					content.erase(i, 1);
					i--;
				}

				// Remove this space if next character is one of those
				if(isblank(content[i]) && i < content.length()-1 && strchr(",)", content[i+1]))
				{
					content.erase(i, 1);
					i--;
				}

				return 0;
			}
		);

		// Remove all blanks at the end
		// Useful for instructions with no arguments
		i = line.content.length() - 1;
		while(i > 0 && i < line.content.length()) // Avoid integer overflow if length is 0
		{
			if(!isblank(line.content[i])) break;

			i--;
		}

		if(i > 0 && i < line.content.length() - 1)
		{
			line.content.erase(i + 1);
		}

		it++;
	}

	// Process macros definitions
	MacroMap macros;
	
	size_t idx = 0;
	while(idx < output.size())
	{
		Line &line = output[idx];

		size_t pos;
		if((pos = line.content.find("#define")) != std::string::npos)
		{
			pos += sizeof("#define"); // size of "#define" *with* NUL character, which counts as the space

			size_t posValue = line.content.find(' ', pos);

			// #define MACRO VALUE
			//         ^pos ^posValue

			std::string name = line.content.substr(pos, posValue - pos);

			macros[name].push_back(line.content.substr(posValue + 1));

			// std::cout << "Defined: \"" << name << "\"\n";

			output.erase(output.begin() + idx); 
			continue; // idx is now the next element
		}
		else if((pos = line.content.find("#macro ")) != std::string::npos) // NOTE: Space is required to not check against #macrogroup
		{
			pos += sizeof("#macro");
			
			std::string name = line.content.substr(pos);

			//#macro MACRO
			//  ..
			//#endmacro

			size_t startOfMacro = idx;

			while(true)
			{
				if(idx == output.size()-1) compile_error(output[startOfMacro].line, "Unfinished macro: %s", name.c_str());

				idx++;
				// line = output[idx];  NOTE: Fuck you
				Line &currentLine = output[idx];

				if(line.content[0] == '#' && find_string(currentLine.content, "#endmacro", "") != std::string::npos) break;

				macros[name].push_back(currentLine.content);
			}

			if(macros[name].size() == 0)
			{
				compile_error(output[idx].line, "Empty macro: %s", name.c_str()); // DONE: Line numbers 
			}

			output.erase(output.begin() + startOfMacro, output.begin() + idx + 1); // Erase #macro, body and #endmacro

			idx = startOfMacro;
		}

		idx++;
	}

	for(idx = 0; idx < output.size(); idx++)
	{
		search_macro(output, idx, macros);
	}

	// TODO: Dealing with single-line macros shouldn't need a wholeass vector

	// Process macro groups
	std::stack<MacroMap> macrogroups;
	size_t macrogroupIndex = 0;
	// Process macro groups
	idx = 0;
	while(idx < output.size())
	{
		Line &line = output[idx];

		if(line.content.find("#macrogroup") != std::string::npos)
		{
			macrogroups.push(MacroMap());
			
			MacroMap &current = macrogroups.top();
			current["GROUP_INDEX"].push_back(std::to_string(++macrogroupIndex));
			current["GROUP_ID"].push_back(random_string(16));

			output.erase(output.begin() + idx); // Erase gives next element
			continue;
		}
		else if(line.content.find("#endmacrogroup") != std::string::npos)
		{
			if(macrogroups.size() == 0) compile_warning(line.line, "Non started macrogroup.");
			
			if(macrogroups.size() > 0)
			{
				macrogroups.pop();

				output.erase(output.begin() + idx);
				continue;
			}
		}
		else if(macrogroups.size() > 0)
		{
			auto &top = macrogroups.top();
			search_macro(output, idx, top);
		}

		idx++;
	}

	// Process other directives

	// Remove empty lines
	std::vector<Line> trimmedOutput;
	trimmedOutput.reserve(output.size());
	
	for(auto &line : output)
	{
		if(!line.content.empty()) trimmedOutput.push_back(std::move(line));
	}

	output = std::move(trimmedOutput);
}
