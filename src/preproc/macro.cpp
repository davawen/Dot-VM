#include "preproc/macro.hpp"

void search_macro(std::vector<Line> &output, size_t idx, const MacroMap &macros)
{
	Line &line = output[idx];

	std::string &str = line.content;

	// Handle macros which take arguments
	size_t macro_start;
	if((macro_start = find_ignore_quotes(str, "#(")) != std::string::npos)
	{
		macro_start += 2;

		// Finding the name of the macro in a first loop is a lot cleaner
		size_t args_start = 0;
		std::string_view name;
		for(size_t i = macro_start; i < str.length(); i++)
		{
			if(str[i] == ' ')
			{
				name = std::string_view(&str[macro_start], i - macro_start);
				args_start = i+1;
				break;
			}
			else if(str[i] == ')')
			{
				goto regular_handle;
			}
		}

		fmt::print("Name: {}\n", name);

		size_t scope = 1;
		size_t arg_num = 1;
		MacroMap args;

		for(size_t i = args_start; i < str.length(); i++)
		{
			if(str[i] == '#' && str[++i] == '(')
			{
				scope++;
			}
			else if(str[i] == ',' || str[i] == ')')
			{
				if(scope == 1)
				{
					args["ARG_" + std::to_string(arg_num)].push_back(str.substr(args_start, i - args_start));

					args_start = i + 1;

					arg_num++;
				}

				if(str[i] == ')') scope--;
			}

			// Left macro
			if(scope == 0) break;
		}

		for(auto &[k, v] : args) { fmt::print("{}: {}\n", k, v[0]); }

		exit(-1);
	}

regular_handle:
	for(const auto &[macroName, macro] : macros)
	{
		size_t index;
		if((index = find_delimited_string(line.content, macroName, " ,\t")) != std::string::npos)
		{
			MacroMap args;

			args["MACRO_INDEX"].push_back(macroName);
			args["MACRO_ID"].push_back(random_string(16));

			line.content.erase(index, macroName.length());
			expand_macro(output, idx, index, macro, args);
		}
		else if((index = line.content.find("#(" + macroName + ")")) != std::string::npos)
		{
			MacroMap args;

			args["MACRO_INDEX"].push_back(macroName);
			args["MACRO_ID"].push_back(random_string(16));

			line.content.erase(index, macroName.length() + 3);
			expand_macro(output, idx, index, macro, args);
		}
		else if((index = line.content.find("#(" + macroName)) != std::string::npos)
		{
			MacroMap args;

			// Recursive search to find the end of the macro
			// (The same as searching for the end of a block and skipping any block along the way)
			// size_t macroEnd = line.content.find(')', index); // TODO: Make due for macros in macros

			size_t macroEnd = index + 2;
			{
				//    #(IF $reg, lt, #(VALUE))
				//                          ^
				//)                         ^
				//#(                                   ^
				//level = 1


				size_t level = 1;
				while(level > 0)
				{
					size_t nextEndParen = line.content.find(')', macroEnd + 1); // If macroEnd is already on a closing parenthesis, this loops forever
					size_t nestedMacro = line.content.find("#(", macroEnd);

					if(nextEndParen == std::string::npos)
					{
						macroEnd = nextEndParen;
						break;
					}

					if(nestedMacro > nextEndParen) // #(MACRO) #(OTHER_MACRO)
					{
						macroEnd = nextEndParen;
						level--;
					}
					else // #(MACRO #(OTHER_MACRO))
					{
						macroEnd = nestedMacro + 2;
						level++;
					}
				}
			}



			// #( MACRO #( ANOTHER_MACRO 1 ) 3 4 )

			if(macroEnd == std::string::npos) compile_error(line, fmt::format("Unfinished expansion at macro {}", macroName));

			//#(MACRO ARG1,ARG2)
			//↑       ↑
			//idx     idx+...+3
			size_t argIdx = 1;
			size_t argPos = index + macroName.length() + 3;

			while(argPos < macroEnd)
			{
				size_t newArgPos = line.content.find(',', argPos);

				if(newArgPos > macroEnd) break;
				if(newArgPos == argPos) compile_error(line, fmt::format("Empty argument at macro expansion %s", macroName));

				args[std::string("ARG_") + std::to_string(argIdx)].push_back(line.content.substr(argPos, newArgPos - argPos));
				
				argPos = newArgPos + 1;
				argIdx++;
			}

			args[std::string("ARG_") + std::to_string(argIdx)].push_back(line.content.substr(argPos, macroEnd - argPos));

			args["NUM_ARGS"].push_back(std::to_string(argIdx));
			args["MACRO_INDEX"].push_back(macroName);
			args["MACRO_ID"].push_back(random_string(16));

			line.content.erase(index, macroEnd - index + 1); // +1 to also erase the end parenthese

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
