#include "parseInstruction.hpp"

Instruction::Type getInstruction(const char *str)
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
			printf("3: %i, 4: %i\n", str[3], str[4]);
			throw std::invalid_argument(std::string("Instruction type does not exist. Got: ") + str);
	}
};

void parseInstructions(const char *filename, std::vector<Instruction> &instructions)
{
	FILE *fp = fopen(filename, "r");
	
	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);
	
	char *buffer = new char[fileSize + 1];
	
	// Copy file to buffer
	char chr;
	int index = 0;
	while((chr = fgetc(fp)) != EOF) // Get new char and check if we reached end of file
	{
		buffer[index++] = chr;
	}
	
	buffer[index] = '\0';
	
	char *next = NULL;
	char *first = strtok_r(buffer, "\n", &next);
	
	do
	{
		char *part;
		char *posn = NULL;
		
		//Loops over the string to find the amount of arguments, and cleans up trailing spaces / commas
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
		int idx = 0;
		
		part = strtok_r(NULL, ",", &posn); // <- Then it becomes the arguments
		while(part != NULL)
		{
			printf("[%s]\n", part);
			arguments[idx++] = strtol(part, NULL, 0); // This shouldn't really ever be called if there are no arguments, so it should be fine ??
			part = strtok_r(NULL, ",", &posn);
		}
		
		printf("Num args: %i, ptr: %p\n", numArgs, (void *)arguments);
		
		instructions.push_back( Instruction( type, arguments, numArgs ) );
	}
	while((first = strtok_r(NULL, "\n", &next)) != NULL);
	
	delete[] buffer;
	
	fclose(fp);
}