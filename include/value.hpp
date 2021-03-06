#pragma once

#include <string>

#include <cstdlib>

struct Value
{
	enum Type
	{
		NUMBER, // 64 bit signed integer
		STRING, // C string (NULL terminated) pointer
		REG, // Register name hash
		OPERATOR, // Dereference ($) operator
		LABEL // Label name hash
	} type;
	
	

	intptr_t val;
	
	Value(Type type = Type::NUMBER, intptr_t val = 0)
	{
		this->type = type;
		this->val = val;
	}
	
	~Value()
	{
		if(type == Type::STRING) delete[] reinterpret_cast<char *>(val);
	}
	
	bool is_register() const
	{
		return type == Type::REG || type == Type::OPERATOR;
	}
};
