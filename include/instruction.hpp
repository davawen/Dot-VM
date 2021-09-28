
#pragma once

#include <iostream>
#include "value.hpp"

struct Instruction
{
	enum struct Type
	{
		PUSH, POP,
		ADD, SUB, MUL, DIV,
		AND, OR, XOR, NOT,
		LSHIFT, RSHIFT,
		MOV,
		LABEL, JUMP, IFEQ,
		PRINT // Will probably get replaced by some sort of sys_call
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
				if(value[i].type == Value::Type::STRING) delete[] reinterpret_cast<char *>(value[i].val);
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
		case Instruction::Type::PUSH:
			os << "push";
			break;
		case Instruction::Type::POP:
			os << "pop";
			break;
		case Instruction::Type::ADD:
			os << "add";
			break;
		case Instruction::Type::SUB:
			os << "sub";
			break;
		case Instruction::Type::MUL:
			os << "mul";
			break;
		case Instruction::Type::DIV:
			os << "div";
			break;
		case Instruction::Type::AND:
			os << "and";
			break;
		case Instruction::Type::OR:
			os << "or";
			break;
		case Instruction::Type::XOR:
			os << "xor";
			break;
		case Instruction::Type::NOT:
			os << "not";
			break;
		case Instruction::Type::LSHIFT:
			os << "lshift";
			break;
		case Instruction::Type::RSHIFT:
			os << "rshift";
			break;
		case Instruction::Type::MOV:
			os << "mov";
			break;
		case Instruction::Type::LABEL:
			os << "label";
			break;
		case Instruction::Type::JUMP:
			os << "jump";
			break;
		case Instruction::Type::IFEQ:
			os << "ifeq";
			break;
		case Instruction::Type::PRINT:
			os << "print";
			break;
	}
	
	if(instruction.value != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < instruction.numValues; i++)
		{
			if(instruction.value[i].type == Value::Type::STRING)
			{
				os << '\"' << reinterpret_cast<char *>(instruction.value[i].val) << "\", ";
			}
			else os << instruction.value[i].val << ", ";
		}
	}
	
	return os;
}
