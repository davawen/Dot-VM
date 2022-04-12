#include "preproc/macro.hpp"

void search_macro(std::vector<Line> &output, size_t idx, const MacroMap &macros)
{
	Line &line = output[idx];

	for(const auto &[macroName, macro] : macros)
	{
		size_t index;
		if((index = find_string(line.content, macroName, " ,\t")) != std::string::npos)
		{
			line.content.erase(index, macroName.length());
			expand_macro(output, idx, index, macro);
		}
		else if((index = line.content.find("#(" + macroName + ")")) != std::string::npos)
		{
			line.content.erase(index, macroName.length() + 3);
			expand_macro(output, idx, index, macro);
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
