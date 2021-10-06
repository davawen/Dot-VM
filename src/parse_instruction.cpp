#include "parse_instruction.hpp"

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

char *handle_escape_sequences(char *str)
{	
	// March along the string and escape escape sequences along the way
	// 'ptr' skips escape sequences, which are overwritten using 'idx'
	
	char *ptr;
	int offset = 0;
	for(ptr = str; *ptr; ptr++)
	{
		char chr = *ptr;

		if(chr == '\\')
		{
			chr = *++ptr;

			if(chr == '\0') throw std::runtime_error("Error: Unterminated escape sequence");
			
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
					goto escapeLoop__;
			}
			
			offset++;

		escapeLoop__:
			;	
		}

		*(ptr - offset) = chr;
	}
	*(ptr - offset) = '\0';

	return str;
}	

void tokenize(const char *filename, std::vector<Token> &tokens)
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
		
			if(chr == ';' && !inQuotation)
			{
				inComment = true;
			}
			else if(chr == '\n')
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
			
			if(chr == '\"' && !inComment)
			{
				inQuotation = !inQuotation;
			}
		}

		// buffer[index] = '\0';
	}
	
	fclose(fp);
	
	char *next = NULL;
	char *first = strtok_r(buffer, "\n", &next);
	
	do
	{
		char *part;
		char *posn = NULL;
		
		//Loops over the string to find the amount of arguments, and cleans up trailing spaces / commas
		// int instructionLength = next - first;

		// TODO: Move this shit to reading
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
					break;
			}
		}
		
		// printf("Num Args: %i\n", numArgs);
		
		part = strtok_r(first, " ", &posn); // <- This give the name of the instruction
		
		if(part == NULL) continue; // Empty line
		
		printf("\x1b[1m\x1b[93m[%s]\x1b[0m\n", part);
		
		tokens.push_back( Token( Token::INSTRUCTION, std::string(part), 0, 0 ) );
		
		part = strtok_r(NULL, ",", &posn); // <- Then it gets the arguments
		
		while(part != NULL)
		{
			if(part[0] == '$') // TODO: recursive dereference
			{
				tokens.push_back( Token( Token::OPERATOR, "$", 0, 0 ) );

				part++;
			}
			
			Token::Type tokenType;
			
			// DONE: Move escape sequences somewhere else to make this work
			if(part[0] == '\"') // overwrite quotation marks if it's a string
			{
				part++;
				char *ptr = part;
				while(*(++ptr + 1));
				
				while(*ptr != '\"') // in case a ',' is in the string
				{
					char *newPart = strtok_r(NULL, ",", &posn);
					
					if(newPart == NULL)
					{
						throw std::runtime_error("Error: non-terminated string");	
					}
					
					// NOTE: This doesn't move anything, just replace the NUL at the end of part with a colon
					// NOTE: So, this could just be replaced by *(newPart - 1) = ','
					part = strcat(part, newPart);
					
					ptr = part;
					while(*(++ptr + 1));
				}
				*(ptr) = '\0'; // reduce length by 1
				
				tokenType = Token::STRING;
			}
			else // DONE: deal with registers (sorta)
			{
				char *endPtr;
				bool isNum = false;
				
				// TODO: add overflow check
				/*int num = */strtoll(part, &endPtr, 0);
				
				isNum = endPtr != part && *endPtr == '\0';

				if(isNum) // Number
				{
					tokenType = Token::NUMBER;
				}
				else // Label or register
				{
					tokenType = Token::LITERAL;
				}

			}
			
			printf("\x1b[33m[%s]\x1b[0m\n", part);
			
			tokens.push_back( Token( tokenType, std::string(part), 0, 0 ) );	
			
			part = strtok_r(NULL, ",", &posn);
		}

		tokens.push_back( Token(Token::NEWLINE, "\n", 0, 0) );
	}
	while((first = strtok_r(NULL, "\n", &next)) != NULL);
	
	delete[] buffer;
	

}

/// DONE: Split this into a lexing step and a parsing step
void parse_instructions(std::vector<Token> &tokens, std::vector<Expression> &expressions)
{
	unsigned long idx = 0;

	while(idx < tokens.size())
	{
		puts("\n");

		Token &token = tokens[idx++];
		
		int numTokens = 0;
		int numArgs = 0;
		while( tokens[idx + numTokens].type != Token::NEWLINE )
		{
			if(tokens[idx + numTokens].type != Token::OPERATOR) numArgs++;
			
			numTokens++;
		}
		
		Instruction::Type instructionType = get_instruction_type(token.value.c_str());
		Value *args = numArgs > 0 ? new Value[numArgs] : nullptr;
		int argIdx = 0;

		// TODO: Recursive dereference
		bool isRegisterValue = false;
		for(int i = 0; i < numTokens; i++)
		{
			Token &curToken = tokens[idx + i];

			switch(curToken.type)
			{
				case Token::NUMBER:
					args[argIdx].val = strtoll(curToken.value.c_str(), NULL, 0);
					args[argIdx].type = Value::NUMBER;

					argIdx++;
					break;
				case Token::STRING:
				{
					std::string *str = new std::string(curToken.value);
					args[argIdx].val = reinterpret_cast<int64_t>(str);
					args[argIdx].type = Value::STRING;
					
					argIdx++;
					break;
				}
				case Token::LITERAL:
					if(instructionType == Instruction::Type::LABEL)
					{
						args[argIdx].val = hash(curToken.value.c_str());
						args[argIdx].type = Value::LABEL; // This should just be set to number
						
						argIdx++;
					}
					else
					{
						switch(hash(curToken.value.c_str()))
						{
							case hash("reg"):
								args[argIdx].val = static_cast<int64_t>(Register::REG);
								break;
							case hash("eax"):
								args[argIdx].val = static_cast<int64_t>(Register::EAX);
								break;
							case hash("sp"):
								args[argIdx].val = static_cast<int64_t>(Register::SP);
								break;
							case hash("void"):
								args[argIdx].val = static_cast<int64_t>(Register::VOID);
								break;
							default:
								compile_error(curToken.line, curToken.pos, "Uknown register: %s", curToken.value.c_str());
								break;
						}

						if(isRegisterValue)
						{
							args[argIdx].type = Value::REG_VALUE;
							isRegisterValue = false;
						}
						else
						{
							args[argIdx].type = Value::REG;
						}

						argIdx++;
					}
					break;
				case Token::OPERATOR:
					isRegisterValue = true;
					break;
				default:
					compile_error(curToken.line, curToken.pos, "Unexpected token encountered: %i, %s", curToken.type, curToken.value.c_str());
					break;
			}
		}
		
		expressions.push_back( Expression( Instruction( instructionType ), args, numArgs )  );
			
		idx += numTokens + 1; // args + new line
	}

	// TODO: Type checking
}
