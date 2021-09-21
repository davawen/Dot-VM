#include <iostream>
#include <vector>
#include <exception>
#include <ostream>

#include <string.h>
#include <time.h>

#include "instruction.hpp"

#define M_LOOP(x) for(int idx__ = 0; idx__ < x; idx__++)

constexpr unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    char c = *str;

    while((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

int str_cut(char *str, int begin, int len)
{
    int l = strlen(str);

    if (len < 0) len = l - begin;
    if (begin + len > l) len = l - begin;
    memmove(str + begin, str + begin + len, l - len + 1);

    return len;
}

int main(int argc, char *argv[])
{
	if(argc <= 1)
	{
		printf("Error: No file specified.\n");
		exit(-1);
	}
	
	FILE *fp = fopen(argv[1], "r");
	
	// Parse file
	std::vector<Instruction> instructions;
	
	{
		auto getInstruction = [](const char *str) -> Instruction::Type
		{
			switch(hash(str))
			{
				case hash("push"):
					return Instruction::Type::push;
				case hash("pop"):
					return Instruction::Type::pop;
				case hash("add"):
					return Instruction::Type::add;
				case hash("ifeq"):
					return Instruction::Type::ifeq;
				case hash("jump"):
					return Instruction::Type::jump;
				case hash("print"):
					return Instruction::Type::print;
				case hash("dup"):
					return Instruction::Type::dup;
				default:
					throw std::invalid_argument(std::string("Instruction type does not exist. Got: ") + str);
			}
		};
		
		fseek(fp, 0L, SEEK_END);
		size_t fileSize = ftell(fp);
		rewind(fp);
		
		char *buffer = new char[fileSize];
		
		// Copy file to buffer
		char chr;
		int index = 0;
		while((chr = fgetc(fp)) != EOF) // Get new char and evaluate it to EOF. This should write NUL to buffer
		{
			buffer[index++] = chr;
		}
		char *next = NULL;
		char *first = strtok_r(buffer, "\n", &next);
		
		do
		{
			char *part;
			char *posn = NULL;
			
			//Loops over the string to find the amount of arguments, and cleans up trailing spaces
			// int instructionLength = next - first;
			int numArgs = 0;
			char *instructionPtr = first;
			while((chr = *instructionPtr++))
			{
				switch(chr)
				{
					case ' ':
						if(*instructionPtr == '\0') // Remove trailing space
						{
							*(instructionPtr-1) = '\0';
						}
						else numArgs++;
						break;
					case ',':
						if(*instructionPtr == '\0')
						{
							*(instructionPtr-1) = '\0';
							break;
						}
						
						if(*instructionPtr == ' ')
						{
							// Move everything to the right of instruction ptr one byte to the left (overwrite space)
							memmove(instructionPtr, instructionPtr + 1, next - instructionPtr); 
						}
						
						numArgs++;
						break;
				}
			}
			
			// printf("Num Args: %i\n", numArgs);
			
			part = strtok_r(first, " ", &posn); // <- This give the name of the instruction
			printf("[%s]\n", part);
			
			Instruction::Type type = getInstruction(part);
			int *arguments = numArgs > 0 ? new int[numArgs] : nullptr;
			printf("Num args: %i, ptr: %p\n", numArgs, (void *)arguments);
			int idx = 0;
			
			part = strtok_r(NULL, ",", &posn); // <- Then it becomes the arguments
			while(part != NULL)
			{
				printf("[%s]\n", part);
				arguments[idx++] = strtol(part, NULL, 0); // This shouldn't really ever be called if there are no arguments, so it should be fine ??
				part = strtok_r(NULL, ",", &posn);
			}
			
			instructions.push_back( Instruction( type, arguments, numArgs ) );
		}
		while((first = strtok_r(NULL, "\n", &next)) != NULL);
	}
	
	fclose(fp);
	
	for(auto &it : instructions)
	{
		std::cout << it << '\n';
	}
	
	// int stack[10000];
	// int *sp = stack;
	
	
	
	return 0;
}
