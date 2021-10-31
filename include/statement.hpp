#pragma once

#include <iostream>

#include "instruction.hpp"
#include "value.hpp"

struct Statement
{
	Instruction ins;
	Value *args;
	int numArgs;
	
	Statement(Instruction ins, Value *value = nullptr, int numValues = 0)
	{
		this->ins = ins;
		this->args = value;
		this->numArgs = numValues;
	}
	
	Statement(Statement &&other)
	{
		ins = other.ins;
		args = other.args;
		numArgs = other.numArgs;
		
		other.args = nullptr;
	}
	
	~Statement()
	{
		if(args != nullptr)
		{
			//for(int i = 0; i < numValues; i++)
			//{
			//}
			
			delete[] args;
		}
	}
};

inline std::ostream &operator<<(std::ostream &os, const Statement &statement)
{
	os << "Instruction: " << Instruction::type_to_name(statement.ins.type);
	
	if(statement.args != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < statement.numArgs; i++)
		{
			if(statement.args[i].type == Value::Type::STRING)
			{
				os << '\"' << reinterpret_cast<char *>(statement.args[i].val) << "\"(Type: ";
			}
			else os << statement.args[i].val << "(Type: ";

			os << statement.args[i].type << "), ";
		}
	}
	
	return os;
}
