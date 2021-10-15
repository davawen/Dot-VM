#pragma once

#include <iostream>

#include "instruction.hpp"
#include "value.hpp"

struct Statement
{
	Instruction ins;
	Value *args; // Should be able to hold a pointer value
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

inline std::ostream &operator<<(std::ostream &os, const Statement &expression)
{
	os << "Instruction: ";
	switch(expression.ins.type)
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
		case Instruction::Type::MOD:
			os << "mod";
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
		case Instruction::Type::CALL:
			os << "call";
			break;
		case Instruction::Type::PRINT:
			os << "print";
			break;
		case Instruction::Type::SYSCALL:
			os << "syscall";
			break;
	}
	
	if(expression.args != nullptr)
	{
		os << ", Value: ";
		for(int i = 0; i < expression.numArgs; i++)
		{
			if(expression.args[i].type == Value::Type::STRING)
			{
				os << '\"' << *reinterpret_cast<std::string *>(expression.args[i].val) << "\"(Type: ";
			}
			else os << expression.args[i].val << "(Type: ";

			os << expression.args[i].type << "), ";
		}
	}
	
	return os;
}
