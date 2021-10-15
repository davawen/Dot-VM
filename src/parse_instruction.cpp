#include "parse_instruction.hpp"
#include "instruction.hpp"

// TODO: Move this to Instruction struct
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
		case hash("mod"):
			return Instruction::Type::MOD;
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
		case hash("call"):
			return Instruction::Type::CALL;
		case hash("print"):
			return Instruction::Type::PRINT;
		case hash("syscall"):
			return Instruction::Type::SYSCALL;
		default:
			compile_error(0, 0, "Instruction type does not exist. Got: %s", str); // Program exits here
			return Instruction::Type::AND; // Make compiler happy
	}
};

std::string &handle_escape_sequences(std::string &str)
{	
	// March along the string and escape escape sequences along the way
	// 'ptr' skips escape sequences, which are overwritten using 'idx'
	
	size_t index = 0;
	size_t offset = 0;
	for(; index < str.length(); index++)
	{
		char chr = str[index];

		if(chr == '\\')
		{
			chr = str[++index];

			if(index >= str.length()) compile_error(0, 0, "Unterminated escape sequence");
			
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

		str[index - offset] = chr;
	}
	str.resize(index - offset);

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
	
	// TODO: Change this to reflect real file positions
	// Maybe using some sort of spacing map ?
	int line = 1;
	do
	{
		char *part;
		char *posn = NULL;
		
		int pos = 0;
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
		
		// printf("\x1b[1m\x1b[93m[%s]\x1b[0m\n", part);

		tokens.push_back( Token( Token::INSTRUCTION, std::string(part), line, pos ) );
		
		pos = posn - first;
		part = strtok_r(NULL, ",", &posn); // <- Then it gets the arguments
		
		while(part != NULL)
		{
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
						compile_error(line, pos + (ptr - part), "Non-terminated string.");
					}
					
					// NOTE: This doesn't move anything, just replace the NUL at the end of part with a colon
					// NOTE: So, this could just be replaced by *(newPart - 1) = ','
					part = strcat(part, newPart);
					
					//ptr = part; // NOTE: This is stupid, it means I'm retraversing the entire string for no fucking reason
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
			
			// printf("\x1b[33m[%s]\x1b[0m\n", part);
			
			tokens.push_back( Token( tokenType, std::string(part), line, pos ) );	
			
			pos = posn - first;
			part = strtok_r(NULL, ",", &posn);
		}

		tokens.push_back( Token(Token::NEWLINE, "\n", line, pos) );

		line++;
	}
	while((first = strtok_r(NULL, "\n", &next)) != NULL);
	
	delete[] buffer;
	

}

/// DONE: Split this into a lexing step and a parsing step
void parse_instructions(std::vector<Token> &tokens, std::vector<Statement> &statements)
{
	unsigned long idx = 0;

	while(idx < tokens.size())
	{
		Token &token = tokens[idx++];
		
		int numArgs = 0;
		while( tokens[idx + numArgs].type != Token::NEWLINE )
		{		
			numArgs++;
		}

		Instruction::Type instructionType = get_instruction_type(token.value.c_str());
		Value *args = numArgs > 0 ? new Value[numArgs] : nullptr;
		int argIdx = 0;

		for(int i = 0; i < numArgs; i++)
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
					args[argIdx].val = reinterpret_cast<intptr_t>(str);
					args[argIdx].type = Value::STRING;
					
					argIdx++;
					break;
				}
				case Token::LITERAL:
					if(instructionType == Instruction::Type::LABEL || instructionType == Instruction::Type::JUMP || instructionType == Instruction::Type::CALL || (instructionType == Instruction::Type::IFEQ && i == 0))
					{
						// TODO: Just store it like a fucking string
						args[argIdx].val = hash(curToken.value.c_str());
						args[argIdx].type = Value::LABEL;
						
						argIdx++;
					}
					else
					{
						bool isRegisterValue = false;

						// TODO: Recursive dereference
						if(curToken.value[0] == '$')
						{
							isRegisterValue = true;

							// Shift by one to the left (remove dollar)
							// Probably not the best way to do it
							curToken.value = std::string( curToken.value.c_str() + 1 );
						}

						switch(hash(curToken.value.c_str()))
						{
							case hash("reg"):
								args[argIdx].val = static_cast<intptr_t>(Register::REG);
								break;
							case hash("eax"):
								args[argIdx].val = static_cast<intptr_t>(Register::EAX);
								break;
							case hash("sp"):
								args[argIdx].val = static_cast<intptr_t>(Register::SP);
								break;
							case hash("void"):
								args[argIdx].val = static_cast<intptr_t>(Register::VOID);
								break;
							default:
								compile_error(curToken.line, curToken.pos, "Uknown register: %s", curToken.value.c_str());
								break;
						}

						if(isRegisterValue)
						{
							args[argIdx].type = Value::REG_VALUE;
						}
						else
						{
							args[argIdx].type = Value::REG;
						}

						argIdx++;
					}
					break;
				default:
					compile_error(curToken.line, curToken.pos, "Unexpected token encountered: %i, %s", curToken.type, curToken.value.c_str());
					break;
			}
		}
		
		statements.push_back( Statement( Instruction( instructionType ), args, numArgs )  );
			
		idx += numArgs + 1; // args + new line
	}

	// TODO: Type checking
}
