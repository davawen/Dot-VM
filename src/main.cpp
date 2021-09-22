#include <cstdio>
#include <iostream>
#include <vector>
#include <exception>
#include <ostream>

#include <cstring>
#include <ctime>

#include "instruction.hpp"
#include "parseInstruction.hpp"

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
	
	parseInstructions(argv[1], instructions);
	
	printf("\n[STARTING EXECUTION FROM HERE]\n\n");

	constexpr int STACK_SIZE = 10000;
	int stack[STACK_SIZE];
	int *sp = stack;
	
	// Register is apparently a reserved c++ keyword *shrugs*
	int reg = 0;

	auto fatalErr = [](const char *str) -> void
	{
		printf("\x1b[31mFatal error: %s.\n", str);
	};

	auto warningErr = [](const char *str) -> void
	{
		printf("\x1b[41mWarning: %s.\n", str);
	};
	
	for(auto iterator = instructions.begin(); iterator != instructions.end(); iterator++)
	{
		auto &it = *iterator;

		// Error checking
		if(sp < stack)
		{
			// TODO: Add flavor text (... at line xx and instruction xx)
			fatalErr("Stack underflow");
		}

		if(sp > stack + STACK_SIZE)
		{
			warningErr("Stack overflow, probable segmentation fault");
		}
		// std::cout << it << '\n';
		
		switch(it.type)
		{
			case Instruction::Type::push:
				if(it.numValues == 0)
				{
					// Push register value instead
					sp++;
					*sp = reg;
				}
				for(int i = 0; i < it.numValues; i++)
				{
					sp++;	
					*sp = it.value[i];
				}
				break;
			case Instruction::Type::pop:
				// Pop current stack value to register
				reg = *sp;
				sp--;
				break;
			case Instruction::Type::dup:
				// Duplicates the value of top of the stack
				sp++;
				*sp = *(sp-1);
				break;
			case Instruction::Type::print:
				if(it.numValues == 0) // Print register
				{
					printf("%i\n", reg);
				}

				for(int i = 0; i < it.numValues; i++)
				{
					printf("%s", reinterpret_cast<char *>(it.value[i]));
				}
				break;
			default:
				break;
		}
	}
	
	
	
	
	
	
	return 0;
}
