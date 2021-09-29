#pragma once

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
	
	Instruction(Instruction::Type type = Instruction::Type::PUSH)
	{
		this->type = type;
	}
};