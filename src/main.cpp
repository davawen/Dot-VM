#include <iostream>
#include <vector>
#include <exception>
#include <ostream>
#include <string.h>
#include <ctime>

struct Instruction
{
	enum struct Type
	{
		push, pop, add, ifeq, jump, print, dup
	};
	
	Type type_;
	int value_;
	
	Instruction(Type type, int value = INT32_MAX)
	{
		type_ = type;
		value_ = value;
	}
};

std::ostream &operator<<(std::ostream &os, const Instruction &instruction)
{
	os << "Instruction: ";
	switch(instruction.type_)
	{
		case Instruction::Type::push:
			os << "push";
			break;
		case Instruction::Type::pop:
			os << "pop";
			break;
		case Instruction::Type::add:
			os << "add";
			break;
		case Instruction::Type::ifeq:
			os << "ifeq";
			break;
		case Instruction::Type::jump:
			os << "jump";
			break;
		case Instruction::Type::print:
			os << "print";
			break;
		case Instruction::Type::dup:
			os << "dup";
			break;
	}
	
	if(instruction.value_ != INT32_MAX) os << " Value: " << instruction.value_;
	
	return os;
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
		std::string current;
		Instruction::Type currentType;
		
		char chr;
		bool isInstruction = true;
		
		while((chr = fgetc(fp)))
		{
			bool isEnd = chr == '\n' || chr == EOF;
			
			if(chr == ' ' || (isEnd && isInstruction))
			{
				if(current == "push") currentType = Instruction::Type::push;
				else if(current == "pop") currentType = Instruction::Type::pop;
				else if(current == "add") currentType = Instruction::Type::add;
				else if(current == "ifeq") currentType = Instruction::Type::ifeq;
				else if(current == "jump") currentType = Instruction::Type::jump;
				else if(current == "print") currentType = Instruction::Type::print;
				else if(current == "dup") currentType = Instruction::Type::dup;
				
				isInstruction = chr != ' ';
				
				current.clear();
			}
			
			switch(chr)
			{
				case EOF:
				case '\n':
					instructions.push_back(
						Instruction(currentType, isInstruction ? INT32_MAX : atoi(current.c_str()))
					);
					
					current.clear();
					isInstruction = true;
					break;
				default:
					current += chr;
					break;
			}
			
			if(chr == EOF) break;
		}
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