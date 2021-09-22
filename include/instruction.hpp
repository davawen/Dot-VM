
#pragma once

#include <iostream>

struct Instruction
{
	enum struct Type
	{
		push, pop, add, ifeq, jump, print, dup, label
	};
	
	Type type;
	int64_t *value; // Should be able to hold a pointer value
	int numValues;
	
	Instruction(Type type, int64_t *value = nullptr, int numValues = 0)
	{
		this->type = type;
		this->value = value;
		this->numValues = numValues;
	}
	
	Instruction(Instruction &&other)
	{
		type = other.type;
		value = other.value;
		numValues = other.numValues;
		
		other.value = nullptr;
	}
	
	~Instruction()
	{
		if(value != nullptr)
		{
			if(type == Type::print)
			{
				for(int i =0; i < numValues; i++)
				{
					delete[] (char *)value[i];
				}
			}
			
			delete[] value;
		}
	}
};

inline std::ostream &operator<<(std::ostream &os, const Instruction &instruction)
{
	os << "Instruction: ";
	switch(instruction.type)
	{
		case Instruction::Type::push:
			os << "push";
			break;
		case Instruction::Type::pop:
			os << "pop";
			break;
		case Instruction::Type::add:
			os << "add";
			break;
		case Instruction::Type::ifeq:
			os << "ifeq";
			break;
		case Instruction::Type::jump:
			os << "jump";
			break;
		case Instruction::Type::print:
			os << "print";
			break;
		case Instruction::Type::dup:
			os << "dup";
			break;
		case Instruction::Type::label:
			os << "label";
			break;
	}
	
	if(instruction.value != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < instruction.numValues; i++)
			os << instruction.value[i] << ", ";
	}
	
	return os;
}
