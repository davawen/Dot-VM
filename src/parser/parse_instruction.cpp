#include "parser/parse_instruction.hpp"
#include "instruction.hpp"

char *handle_escape_sequences(char *str)
{	
	// March along the string and escape escape sequences along the way
	// 'ptr' skips escape sequences, which are overwritten using 'idx'
	
	size_t index = 0;
	size_t offset = 0;
	for(; str[index] != '\0'; index++)
	{
		char chr = str[index];

		if(chr == '\\')
		{
			chr = str[++index];

			if(chr == '\0') compile_error(0, "Unterminated escape sequence"); // TODO: Figure out how to pass line / char number appropriately
			
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
	str[index - offset] = '\0';

	return str;
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

		Instruction::Type instructionType = Instruction::name_to_type(token.value);
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
					char *str = new char[curToken.value.size() + 1];

					const char *curStr = curToken.value.c_str();
					strcpy(str, curStr);

					handle_escape_sequences(str);

					args[argIdx].val = reinterpret_cast<intptr_t>(str);
					args[argIdx].type = Value::STRING;
					
					argIdx++;
					break;
				}
				case Token::OPERATOR:
					args[argIdx].val = 0; // TODO: Operator enum
					args[argIdx].type = Value::OPERATOR;

					argIdx++;
					break;
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
						// DONE: Recursive dereference
						switch(hash(curToken.value.c_str()))
						{
							case hash("reg"):
								args[argIdx].val = static_cast<intptr_t>(Register::REG);
								break;
							case hash("rax"):
								args[argIdx].val = static_cast<intptr_t>(Register::RAX);
								break;
							case hash("rcx"):
								args[argIdx].val = static_cast<intptr_t>(Register::RCX);
								break;
							case hash("sp"):
								args[argIdx].val = static_cast<intptr_t>(Register::SP);
								break;
							default:
								compile_error(curToken.line, fmt::format("Uknown register: {}", curToken.value));
								break;
						}

						args[argIdx].type = Value::REG;

						argIdx++;
					}
					break;
				default:
					compile_error(curToken.line, fmt::format("Unexpected token encountered: {}, {}", curToken.type, curToken.value));
					break;
			}
		}

		statements.push_back( Statement( Instruction( instructionType ), args, numArgs )  );
			
		idx += numArgs + 1; // args + new line
	}

	// TODO: Type checking
}
