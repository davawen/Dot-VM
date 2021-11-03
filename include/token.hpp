#pragma once

#include <string>

struct Token
{
	enum Type
	{
		INSTRUCTION, // Pretty straigth forward
		LITERAL, // Registers, Labels
		OPERATOR,
		NUMBER, 
		STRING,
		NEWLINE // End of current instruction
	} type;

	std::string value;

	/// Length of the string 'value'
	
	int line;
	int pos;

	Token()
	{
		this->type = Type::INSTRUCTION;
		this->value = "";
		this->line = 0;
		this->pos = 0;
	}

	Token(Type type, std::string value, int line, int pos)
	{
		this->type = type;
		this->value = value;
		this->line = line;
		this->pos = pos;
	}
};
