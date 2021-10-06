#include <iostream>
#include <vector>
#include <ostream>
#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <exception>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <cinttypes>

#include "instruction.hpp"
#include "parse_instruction.hpp"

#include "interpreter.hpp"

#define FLAG_IMPLEMENTATION
#include "flags.hpp"

namespace fs = std::filesystem;

int main(int argc, char **argv)
{
	bool *help = flag_bool("help", "Prints this help and exits.", false);
	
	const char **file = flag_str("file", "Input file.", NULL);
	bool *comp = flag_bool("comp", "Wether the program should get compiled. [WIP]", false);

	parse_flags(argc, argv);
	
	if(*help || argc == 1)
	{
		print_help("dot-vm --file FILENAME [--comp]", "Takes a dotvm file and either interpret it or compiles it to a binary.");

		return EXIT_SUCCESS;
	}

	if(*file == NULL)
	{
		printf("Error: no input file recieved.\n");
		return EXIT_FAILURE;
	}
	
	if(!fs::exists(*file) || !fs::is_regular_file(*file))
	{
		printf("Error: incorrect file given.\n");
		return EXIT_FAILURE;
	}
	
	
	std::vector<Token> tokens;

	tokenize(*file, tokens);

	for(auto &token : tokens)
	{
		printf("Token, type: %i, value: %s\n", token.type, token.value.c_str());
	}

	// Parse file
	std::vector<Expression> expressions;

	parse_instructions(tokens, expressions);
	

	//printf("\n");
	std::for_each(expressions.begin(), expressions.end(), [](Expression &ins){ std::cout << ins << '\n'; });
	//
	

	//printf("\n[STARTING EXECUTION FROM HERE]\n\n");
	//
	//interpret(expressions);

	return 0;
}
