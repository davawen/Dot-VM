#include "preproc/macro.hpp"

void search_macro(std::vector<Line> &output, size_t idx, const MacroMap &macros)
{
	Line &line = output[idx];

	std::string &str = line.content;

	// Handle macros which take arguments
	size_t macro_start = 0;
	while((macro_start = find_ignore_quotes(str, "#(", macro_start)) != std::string::npos)
	{
		size_t name_start = macro_start + 2;

		// fmt::print("Line: {}\n", str);

		// Finding the name of the macro in a first loop is a lot cleaner
		size_t scope = 1;
		size_t next_arg = std::string::npos;
		size_t macro_end = std::string::npos;

		std::string name;
		for(size_t i = name_start; i < str.length(); i++)
		{
			if(str[i] == ' ')
			{
				name = string_slice(str, name_start, i);
				next_arg = i+1;
				break;
			}
			else if(str[i] == ')')
			{
				name = string_slice(str, name_start, i);
				// next_arg = std::string::npos; // Avoid iterating the string to find (non existent) args
				scope = 0;
				macro_end = i + 1;
				break;
			}
		}

		// fmt::print("Name: {}\n", name);

		size_t arg_num = 0;
		MacroMap args;

		for(size_t i = next_arg; i < str.length(); i++)
		{
			if(str[i] == '#' && str[++i] == '(')
			{
				scope++;
			}
			else if(str[i] == ',' || str[i] == ')')
			{
				if(scope == 1)
				{
					args["ARG_" + std::to_string(++arg_num)].push_back(string_slice(str, next_arg, i));

					next_arg = i + 1;
					macro_end = next_arg;
				}

				if(str[i] == ')') scope--;
			}

			// Left macro
			if(scope == 0) break;
		}

		if(scope != 0) compile_error(line, fmt::format("Unfinished macro expansion: {}", name));

		auto macro = macros.find(name);

		if(macro == macros.end())
		{
			macro_start = macro_end;
			continue;
		}

		// TODO: Currently incompatible with the way args are processed... I think...
		// Could have checked for invalid name earlier, but it would have probably triggered in the case of an unfinished macro, so it's better to see after it
		// if(macros.find(name) == macros.end()) compile_error(line, fmt::format("Uknown macro in expansion: {}", name));

		args["NUM_ARGS"].push_back(std::to_string(arg_num));
		args["MACRO_INDEX"].push_back(name);
		args["MACRO_ID"].push_back(random_string(16));

		/* for(auto &[k, v] : args) { fmt::print("{}: {}\n", k, v[0]); }
		fmt::print("\n"); */

		str.erase(macro_start, macro_end - macro_start);
		expand_macro(output, idx, macro_start, macro->second, args);
	}

	// Handle regular (without guards) macros
	for(const auto &[macroName, macro] : macros)
	{
		size_t index;
		if((index = find_delimited_string(str, macroName, " ,\t")) != std::string::npos)
		{
			MacroMap args;

			args["NUM_ARGS"].push_back("0");
			args["MACRO_INDEX"].push_back(macroName);
			args["MACRO_ID"].push_back(random_string(16));

			str.erase(index, macroName.length());
			expand_macro(output, idx, index, macro, args);
		}
	}
}

void expand_macro(std::vector<Line> &output, size_t idx, const size_t pos, const Macro &macro)
{
	Line &line = output[idx];

	line.content.insert(pos, macro[0]);

	if(macro.size() > 1)
	{
		output.insert(output.begin() + idx + 1, macro.size() - 1, Line("", line.file, line.line));
		for(size_t i = 1; i < macro.size(); i++)
		{
			output[idx + i].content = macro[i];
		}
	}
}

void expand_macro(std::vector<Line> &output, size_t idx, const size_t pos, const Macro &macro, const MacroMap &args)
{
	Line &line = output[idx];

	line.content.insert(pos, macro[0]);

	if(macro.size() > 1)
	{
		output.insert(output.begin() + idx + 1, macro.size() - 1, Line("", line.file, line.line));
		for(size_t i = 1; i < macro.size(); i++)
		{
			output[idx + i].content = macro[i];
		}
	}
	for(size_t i = 0; i < macro.size(); i++)
	{
		search_macro(output, idx, args);

		idx++;
	}
}
