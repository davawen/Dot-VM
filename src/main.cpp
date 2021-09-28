#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <exception>
#include <ostream>
#include <algorithm>

#include <cstring>
#include <ctime>
#include <cinttypes>

#include "instruction.hpp"
#include "parseInstruction.hpp"

#include "interpreter.hpp"

#define M_LOOP(x) for(int idx__ = 0; idx__ < x; idx__++)

// int str_cut(char *str, int begin, int len)
// {
//     int l = strlen(str);

//     if (len < 0) len = l - begin;
//     if (begin + len > l) len = l - begin;
//     memmove(str + begin, str + begin + len, l - len + 1);

//     return len;
// }

int main(int argc, char *argv[])
{
	if(argc <= 1)
	{
		printf("Error: No file specified.\n");
		exit(-1);
	}
	
	
	// Parse file
	std::vector<Instruction> instructions;
	
	parse_instructions(argv[1], instructions);
	

	printf("\n");
	std::for_each(instructions.begin(), instructions.end(), [](Instruction &ins){ std::cout << ins << '\n'; });
	

	printf("\n[STARTING EXECUTION FROM HERE]\n\n");
	
	interpret(instructions);

	return 0;
}
