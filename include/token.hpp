#pragma once

#include <string>

struct Token
{
	enum Type
	{
		INSTRUCTION, // Pretty straigth forward
		OPERATOR, // -> Operator $
		LITERAL, // Registers, Labels
		NUMBER, 
		STRING,
		NEWLINE // End of current instruction
	} type;

	std::string value;

	/// Length of the string 'value'
	
	int pos;
	int line;

	Token()
	{
		this->type = Type::INSTRUCTION;
		this->value = "";
		this->pos = 0;
		this->line = 0;
	}

	Token(Type type, std::string value, int pos, int line)
	{
		this->type = type;
		this->value = value;
		this->pos = pos;
		this->line = line;
	}
};
