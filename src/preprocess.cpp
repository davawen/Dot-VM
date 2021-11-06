#include "preprocess.hpp"

static void read_file(const char *filename, std::list<std::string> &output)
{
	std::ifstream file(filename);
	std::string line;

	while(std::getline(file, line))
	{
		output.push_back(line);
	}
}

void preprocess(const char *filename, std::list<std::string> &output)
{
	read_file(filename, output);
	
	std::string filenameDirectory = filename;
	filenameDirectory.erase(filenameDirectory.find_last_of('/') + 1);
	
	// Include directives
	auto it = output.begin();
	std::unordered_set<std::string> includedFiles;
	
	while(it != output.end())
	{
		std::string &line = *it;

		if(line[0] == '#')
		{
			// Chech for preprocessor directives

			if(line.find("#include") != std::string::npos)
			{
				// TODO: Make marking files more robust
				std::string includeFile = line.substr(line.find("\"") + 1, line.rfind("\"") - line.find("\"") - 1);
				if(line.find("#include_recursive") == std::string::npos && includedFiles.find(includeFile) != includedFiles.end())
				{
					// Already included
					it = output.erase(it);
					continue;
				}

				includedFiles.insert(includeFile);

				includeFile = filenameDirectory + includeFile;

				std::list<std::string> include_output;
				read_file(includeFile.c_str(), include_output);

				output.insert(it, include_output.begin(), include_output.end());
				it = output.erase(it);
				it--;
			}
		}

		++it;
	}
	
	// Remove trailing spaces / comments
	size_t lineNum = 1;
	it = output.begin();
	while(it != output.end())
	{
		std::string &line = *it;
		
		// Remove comments
		if(line.find_first_of(';') != std::string::npos) line.erase(line.find_first_of(';'));

		// Remove blanks at the beginning
		size_t i = 0;
		while(true)
		{
			if(!isblank(line[i])) break;

			i++;
		}

		if(i > 0)
		{
			line.erase(0, i);
		}

		// Remove blanks between arguments
		while(i < line.length())
		{
			if(isblank(line[i])) break; // Skip first argument

			i++;
		}

		if(i != line.length())
		{
			i++;

			bool inQuotes = false;

			while(i < line.length())
			{
				if(inQuotes)
				{
					if(line[i] == '\\')
					{
						i++;
					}
					else if(line[i] == '\"')
					{
						inQuotes = false;
					}
				}
				else if(line[i] == '"')
				{
					inQuotes = true;
				}
				else if(isblank(line[i]))
				{
					line.erase(i, 1);
					i--;
				}

				i++;
			}
		}

		// Remove all blanks at the end
		// Useful for instructions with no arguments
		i = line.length() - 1;
		printf("%zu  %lu\n", i, line.length());
		while(i > 0 && i < line.length()) // Avoid integer overflow if length is 0
		{
			if(!isblank(line[i])) break;

			i--;
		}

		
		if(i > 0 && i < line.length() - 1)
		{
			line.erase(i + 1);
		}

		it++;
		lineNum++;
	}

	// Process macros definitions

	// Expand macros

	// Process macro groups

	// Process other directives
}
