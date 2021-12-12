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

/// Search string 'str' for the string 'query', which is either have the the given delimeters on its side, or be on the borders of the string 
/// Returns the index where query starts if it was found, and std::string::npos otherwise
static size_t find_string(const std::string &str, const std::string &query, const char *delimeters)
{
	size_t i = 0;
	while((i = str.find(query, i)) != std::string::npos)
	{
		if((i == 0 || strchr(delimeters, str[i - 1])) && ((i + query.length()) >= str.length() || strchr(delimeters, str[i + query.length()])))
		{
			return i;
		}
		else
		{
			i++;
		}
	}

	return std::string::npos;
}

static std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

void search_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const MacroMap &macros)
{
	Line &line = *it;

	for(auto &pair : macros)
	{
		size_t index;
		if((index = find_string(line.content, pair.first, " ,\t")) != std::string::npos)
		{
			line.content.erase(index, pair.first.length());
			expand_simple_macro(output, it, index, macros, pair.second);
		}
		else if((index = line.content.find("#(" + pair.first + ")")) != std::string::npos)
		{
			line.content.erase(index, pair.first.length() + 3);
			expand_simple_macro(output, it, index, macros, pair.second);
		}
		else if((index = line.content.find("#(" + pair.first)) != std::string::npos)
		{
			MacroMap args;
			size_t macroEnd = line.content.find(')', index); // TODO: Make due for macros in macros

			if(macroEnd == std::string::npos) compile_error(line.line, "Unfinished expansion at macro %s", pair.first.c_str());

			//#(MACRO ARG1,ARG2)
			//↑       ↑
			//idx     idx+...+3
			size_t argIdx = 1;
			size_t argPos = index + pair.first.length() + 3;

			while(argPos < macroEnd)
			{
				size_t newArgPos = line.content.find(',', argPos);

				if(newArgPos > macroEnd) break;
				if(newArgPos == argPos) compile_error(line.line, "Empty argument at macro expansion %s", pair.first.c_str());

				args[std::string("ARG_") + std::to_string(argIdx)].push_back(line.content.substr(argPos, newArgPos - argPos));
				
				argPos = newArgPos + 1;
				argIdx++;
			}

			args[std::string("ARG_") + std::to_string(argIdx)].push_back(line.content.substr(argPos, macroEnd - argPos));

			args["NUM_ARGS"].push_back(std::to_string(argIdx));
			args["MACRO_INDEX"].push_back(pair.first);
			args["MACRO_ID"].push_back(random_string(16));

			line.content.erase(index, macroEnd - index + 1); // +1 to also erase the end parenthese

			expand_macro(output, it, index, macros, pair.second, args);
		}
	}
}

void expand_simple_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const size_t pos, const MacroMap &macros, const Macro &macro)
{
	Line &line = *it;

	line.content.insert(pos, macro[0]);

	if(macro.size() > 1)
	{
		output.insert(it + 1, macro.size() - 1, Line("", line.file, line.line));
		for(size_t i = 1; i < macro.size(); i++)
		{
			(it + i)->content = macro[i];
		}
	}

	for(size_t i = 0; i < macro.size(); i++)
	{
		search_macro(output, it, macros);

		it++;
	}
}

void expand_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const size_t pos, const MacroMap &macros, const Macro &macro, const MacroMap &args)
{
	Line &line = *it;

	line.content.insert(pos, macro[0]);

	if(macro.size() > 1)
	{
		output.insert(it + 1, macro.size() - 1, Line("", line.file, line.line));
		for(size_t i = 1; i < macro.size(); i++)
		{
			(it + i)->content = macro[i];
		}
	}

	for(size_t i = 0; i < macro.size(); i++)
	{
		search_macro(output, it, args);
		search_macro(output, it, macros);

		it++;
	}
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

				it = output.insert(it, include_output.begin(), include_output.end());

				it--; // Counter-balance it++
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
	
	it = output.begin();
	while(it != output.end())
	{
		Line &line = *it;

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

			it = --output.erase(it);
		}
		else if((pos = line.content.find("#macro ")) != std::string::npos) // NOTE: Space is required to not check against #macrogroup
		{
			pos += sizeof("#macro");
			
			std::string name = line.content.substr(pos);

			//#macro MACRO
			//  ..
			//#endmacro

			auto startOfMacro = it;

			while(true)
			{
				it++;
				line = *it;

				if(line.content[0] == '#' && find_string(line.content, "#endmacro", "") != std::string::npos) break;

				macros[name].push_back(line.content);
			}

			if(macros[name].size() == 0)
			{
				compile_error(line.line, "Empty macro %s", name.c_str()); // DONE: Line numbers 
			}

			// std::cout << "Defined: \"" << name << "\n";

			it = output.erase(startOfMacro, it + 1) - 1;
		}
		else
		{
			search_macro(output, it, macros);
		}

		it++;
	}

	/*for(auto &pair: macros)
	{
		std::cout << "Macro: \"" << pair.first << "\"\n{\n";
		for(auto &str : pair.second)
		{
			std::cout << "\t" << str << "\n";
		}
		std::cout << "}\n";
	}*/

	// Process macro groups

	// Process other directives
}
