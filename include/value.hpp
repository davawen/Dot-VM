#pragma once

#include <cstdlib>

struct Value
{
	enum Type
	{
		value, // 64 bit signed integer
		string, // C string (NULL terminated) pointer
		reg, // Register name hash
		regValue, // Register name hash
		label // Label name hash
	} type;
	
	int64_t val;
	
	int64_t operator=(int64_t val)
	{
		this->val = val;
		return val;
	}
};