
#pragma once

#include <iostream>
#include "value.hpp"

struct Instruction
{
	enum struct Type
	{
		push, pop,
		add, sub, mul, div,
		and_, or_, xor_, not_, // Reserved names
		lshift, rshift,
		mov,
		label, jump, ifeq,
		print // Will probably get replaced by some sort of sys_call
	};
	
	Type type;
	Value *value; // Should be able to hold a pointer value
	int numValues;
	
	Instruction(Type type, Value *value = nullptr, int numValues = 0)
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
			for(int i = 0; i < numValues; i++)
			{
				if(value[i].type == Value::Type::string) delete[] reinterpret_cast<char *>(value[i].val);
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
		case Instruction::Type::sub:
			os << "sub";
			break;
		case Instruction::Type::mul:
			os << "mul";
			break;
		case Instruction::Type::div:
			os << "div";
			break;
		case Instruction::Type::and_:
			os << "and";
			break;
		case Instruction::Type::or_:
			os << "or";
			break;
		case Instruction::Type::xor_:
			os << "xor";
			break;
		case Instruction::Type::not_:
			os << "not";
			break;
		case Instruction::Type::lshift:
			os << "lshift";
			break;
		case Instruction::Type::rshift:
			os << "rshift";
			break;
		case Instruction::Type::mov:
			os << "mov";
			break;
		case Instruction::Type::label:
			os << "label";
			break;
		case Instruction::Type::jump:
			os << "jump";
			break;
		case Instruction::Type::ifeq:
			os << "ifeq";
			break;
		case Instruction::Type::print:
			os << "print";
			break;
	}
	
	if(instruction.value != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < instruction.numValues; i++)
		{
			if(instruction.value[i].type == Value::Type::string)
			{
				os << '\"' << reinterpret_cast<char *>(instruction.value[i].val) << "\", ";
			}
			else os << instruction.value[i].val << ", ";
		}
	}
	
	return os;
}
