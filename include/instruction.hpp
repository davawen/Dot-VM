
#pragma once

#include <iostream>
struct Instruction
{
	enum struct Type
	{
		push, pop, add, ifeq, jump, print, dup
	};
	
	Type type_;
	int *value_;
	int numValues_;
	
	Instruction(Type type, int *value = nullptr, int numValues = 0)
	{
		type_ = type;
		value_ = value;
		numValues_ = numValues;
	}
	
	Instruction(Instruction &&other)
	{
		type_ = other.type_;
		value_ = other.value_;
		numValues_ = other.numValues_;
		
		other.value_ = nullptr;
	}
	
	~Instruction()
	{
		if(value_ != nullptr) delete[] value_;
	}
};

inline std::ostream &operator<<(std::ostream &os, const Instruction &instruction)
{
	os << "Instruction: ";
	switch(instruction.type_)
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
	}
	
	if(instruction.value_ != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < instruction.numValues_; i++)
			os << instruction.value_[i] << ", ";
	}
	
	return os;
}
