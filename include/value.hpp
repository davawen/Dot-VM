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
		REG_VALUE, // Register name hash
		LABEL // Label name hash
	} type;
	
	int64_t val;
	
	Value(Type type = Type::NUMBER, int64_t val = 0)
	{
		this->type = type;
		this->val = val;
	}
	
	~Value()
	{
		//if(type == Type::STRING) delete[] reinterpret_cast<char *>(value[i].val);
		if(type == Type::STRING) delete reinterpret_cast<std::string *>(val);
	}

	int64_t operator=(int64_t val)
	{
		this->val = val;
		return val;
	}
};