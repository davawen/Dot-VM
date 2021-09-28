#include "parseInstruction.hpp"

Instruction::Type get_instruction_type(const char *str)
{
	switch(hash(str))
	{
		case hash("push"):
			return Instruction::Type::PUSH;
		case hash("pop"):
			return Instruction::Type::POP;
		case hash("add"):
			return Instruction::Type::ADD;
		case hash("sub"):
			return Instruction::Type::SUB;
		case hash("mul"):
			return Instruction::Type::MUL;
		case hash("div"):
			return Instruction::Type::DIV;
		case hash("and"):
			return Instruction::Type::AND;
		case hash("or"):
			return Instruction::Type::OR;
		case hash("xor"):
			return Instruction::Type::XOR;
		case hash("not"):
			return Instruction::Type::NOT;
		case hash("lshift"):
			return Instruction::Type::LSHIFT;
		case hash("rshift"):
			return Instruction::Type::RSHIFT;
		case hash("mov"):
			return Instruction::Type::MOV;
		case hash(":"):
			return Instruction::Type::LABEL;
		case hash("jump"):
			return Instruction::Type::JUMP;
		case hash("ifeq"):
			return Instruction::Type::IFEQ;
		case hash("print"):
			return Instruction::Type::PRINT;
		default:
			throw std::invalid_argument(std::string("Instruction type does not exist. Got: ") + str);
	}
};

// void validateArguments()
// {
	
// }

void parse_instructions(const char *filename, std::vector<Instruction> &instructions)
{
	FILE *fp = fopen(filename, "r");
	
	fseek(fp, 0L, SEEK_END);
	size_t fileSize = ftell(fp);
	rewind(fp);
	
	// Initialize buffer to NULL
	char *buffer = new char[fileSize + 1];
	memset(buffer, '\0', fileSize + 1);

	char chr;
	// Copy file to buffer
	{
		// Preliminary parsing
		// Remove duplicate spaces and comments
		// Evaluate escape sequences
		bool wasBlank = false, inQuotation = false, inComment = false, startOfLine = false;
		
		int index = 0;
		while((chr = fgetc(fp)) != EOF) // Get new char and check if we reached end of file
		{
			bool escaped = false;

			// Escape characters
			if(chr == '\\')
			{
				chr = fgetc(fp);

				if(chr == EOF) break;
				
				escaped = true;
				/*
					TODO: 
						Putting escape sequences here messes everything later during tokenization
						
						They should be put at the end of the lexer, or shoudl be delt with during runtime / compilation
				*/
				switch(chr)
				{
					case 'a':
						chr = '\a';
						break;
					case 'b':
						chr = '\b';
						break;
					case 'f':
						chr = '\f';
						break;
					case 'n':
						chr = '\n';
						break;
					case 'r':
						chr = '\r';
						break;
					case 't':
						chr = '\t';
						break;
					case 'v':
						chr = '\v';
						break;
					case '\"':
						chr = '\"';
						break;
					case '?':
						chr = '\?';
						break;
					case '\\': // chr is already a backslash
						break;
					default:
						printf("\x1b[31m[Warning]\x1b[0m : uknown escape sequence : \\%c", chr);
						break;
				}
			}
		
			if(chr == ';' && !inQuotation)
			{
				inComment = true;
			}
			else if(chr == '\n' && !escaped)
			{
				inComment = false;
				
				startOfLine = true;

				buffer[index++] = chr;
				continue;
			}

			if(!inQuotation && !inComment)
			{
				bool isBlank = isblank(chr);

				if(isBlank && (wasBlank || startOfLine)) continue;
				else if(isBlank && !wasBlank) wasBlank = true;
				else wasBlank = false;
								
				startOfLine = false;
				buffer[index++] = chr;
			}
			else if(inQuotation) buffer[index++] = chr;
			
			if(chr == '\"' && !inComment && !escaped)
			{
				inQuotation = !inQuotation;
			}
		}

		// buffer[index] = '\0';
	}
	
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
					else if(numArgs == 0) numArgs++; // Only counts first space as an argument
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
		
		if(part == NULL) continue; // Empty line
		
		printf("\x1b[1m\x1b[93m[%s]\x1b[0m\n", part);
		
		Instruction::Type type = get_instruction_type(part);
		Value *arguments = numArgs > 0 ? new Value[numArgs] : nullptr;
		int idx = 0;
		
		part = strtok_r(NULL, ",", &posn); // <- Then it gets the arguments
		
		while(part != NULL) // This shouldn't really ever be called if there are no arguments, so it should be fine ??
		{
			bool isString = false;
			int partLength;
			
			// TODO: Move escape sequences somewhere else to make this work
			if(part[0] == '\"') // overwrite quotation marks if it's a string
			{
				part++;
				char *ptr = part;
				while(*(ptr++ + 2)); // iterate until before NULL character
				
				while(*ptr != '\"') // in case a , is in the string
				{
					char *newPart = strtok_r(NULL, ",", &posn);
					
					if(newPart == NULL)
					{
						throw std::runtime_error("Lexer Error : non-terminated string");	
					}
					
					part = strcat(part, newPart);
					
					char *ptr = part;
					while(*(ptr++ + 2)); // iterate until before NULL character
				}
				*(ptr) = '\0'; // reduce length by 1
				
				partLength = ptr - part;
				isString = true;
				
				arguments[idx].type = Value::Type::STRING;
			}
			else // TODO: deal with register values
			{
				partLength = strlen(part);
				
				arguments[idx].type = Value::Type::VALUE;
			}
			
			printf("\x1b[33m[%s]\x1b[0m\n", part);
			switch(type)
			{
				case Instruction::Type::JUMP:
				case Instruction::Type::LABEL:
					arguments[idx] = hash(part);
					break;
				// Be warned : this is going to be weird as fuck.
				case Instruction::Type::PRINT:
					if(isString)
					{
						char *strPtr = new char[partLength];
						memcpy(strPtr, part, partLength);
						
						// Stores pointer in int64_t
						// Keeps bit configuration intact, will be converted back to pointer when printed out
						// Ex with 8 bit pointers :
						// char * : 1001 0110 (0x96) <-> int64_t : 1001 0110 (-0x6A)
						arguments[idx] = reinterpret_cast<int64_t>(strPtr); 
					}
					else
					{
						arguments[idx] = strtoll(part, NULL, 0);
					}
					
					break;
				default:
					arguments[idx] = strtoll(part, NULL, 0); 
					break;
			}
			
			part = strtok_r(NULL, ",", &posn);
			idx++;
		}
		
		printf("Num args: %i, ptr: %p\n", numArgs, (void *)arguments);
		
		instructions.push_back( Instruction( type, arguments, numArgs ) );
	}
	while((first = strtok_r(NULL, "\n", &next)) != NULL);
	
	delete[] buffer;
	
	fclose(fp);
}
