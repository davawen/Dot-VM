#pragma once

#include <cstdlib>

struct Value
{
	enum Type
	{
		VALUE, // 64 bit signed integer
		STRING, // C string (NULL terminated) pointer
		REG, // Register name hash
		REG_VALUE, // Register name hash
		LABEL // Label name hash
	} type;
	
	int64_t val;
	
	Value(Type type = Type::VALUE, int64_t val = 0)
	{
		this->type = type;
		this->val = val;
	}
	
	int64_t operator=(int64_t val)
	{
		this->val = val;
		return val;
	}
};