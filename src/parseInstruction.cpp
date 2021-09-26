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
		case hash(":"):
			return Instruction::Type::label;
		default:
			throw std::invalid_argument(std::string("Instruction type does not exist. Got: ") + str);
	}
};

void validateArguments()
{
	
}

void parseInstructions(const char *filename, std::vector<Instruction> &instructions)
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

			if(chr == '\"' && !inComment && !escaped)
			{
				inQuotation = !inQuotation;
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
				// case '\\':
				// {
				// 	switch(*instructionPtr)
				// 	{
				// 		case 'a':
				// 			*instructionPtr = '\a';
				// 			break;
				// 		case 'b':
				// 			*instructionPtr = '\b';
				// 			break;
				// 		case 'f':
				// 			*instructionPtr = '\f';
				// 			break;
				// 		case 'n':
				// 			*instructionPtr = '\n';
				// 			break;
				// 		case 'r':
				// 			*instructionPtr = '\r';
				// 			break;
				// 		case 't':
				// 			*instructionPtr = '\t';
				// 			break;
				// 		case 'v':
				// 			*instructionPtr = '\v';
				// 			break;
				// 		case '?':
				// 			*instructionPtr = '\?';
				// 			break;
				// 		case '\\': // Simply avoid warning
				// 			break;
				// 		default:
				// 			printf("\x1b[31m[Warning]\x1b[0m : uknown escape sequence : \\%c", *instructionPtr);
				// 			goto escape;
				// 			break;
				// 	}
				// 	
				// 	// Move instruction ptr one byte to the left (overwrite backward slash)
				// 	memmove(instructionPtr - 1, instructionPtr, next - instructionPtr);
				// 	
				// 	escape:
				// 	
				// 	break;
				// }
				// case '#': // Comment
				// 	printf("\x1b[36m[Comment] : %s\x1b[0m\n", instructionPtr);
				// 	*(instructionPtr-1) = '\0'; // Replace '#' char to NULL
				// 	*instructionPtr = '\0'; // Stop next iteration of the loop
				// 	break;
			}
		}
		
		// printf("Num Args: %i\n", numArgs);
		
		part = strtok_r(first, " ", &posn); // <- This give the name of the instruction
		printf("\x1b[1m\x1b[93m[%s]\x1b[0m\n", part);
		
		Instruction::Type type = getInstruction(part);
		int64_t *arguments = numArgs > 0 ? new int64_t[numArgs] : nullptr;
		int idx = 0;
		
		part = strtok_r(NULL, ",", &posn); // <- Then it gets the arguments
		
		while(part != NULL) // This shouldn't really ever be called if there are no arguments, so it should be fine ??
		{
			printf("\x1b[33m[%s]\x1b[0m\n", part);
			switch(type)
			{
				case Instruction::Type::jump:
				case Instruction::Type::label:
					arguments[idx++] = hash(part);
					break;
				// Be warned : this is going to be weird as fuck.
				case Instruction::Type::print:
				{
					int partLength = strlen(part) + 1; 
					char *strPtr = new char[partLength];
					memcpy(strPtr, part, partLength);
					
					// Stores pointer in int64_t
					// Keeps bit configuration intact, will be converted back to pointer when printed out
					// Ex with 8 bit pointers :
					// char * : 1001 0110 (0x96) <-> int64_t : 1001 0110 (-0x6A)
					arguments[idx++] = reinterpret_cast<int64_t>(strPtr); 
					
					break;
				}
				default:
					arguments[idx++] = strtol(part, NULL, 0); 
					break;
			}
			
			part = strtok_r(NULL, ",", &posn);
		}
		
		printf("Num args: %i, ptr: %p\n", numArgs, (void *)arguments);
		
		instructions.push_back( Instruction( type, arguments, numArgs ) );
	}
	while((first = strtok_r(NULL, "\n", &next)) != NULL);
	
	delete[] buffer;
	
	fclose(fp);
}
