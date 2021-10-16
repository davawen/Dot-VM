#pragma once
#include <string>
#include <unordered_map>

#include "hash.hpp"
#include "error_handling.hpp"

struct Instruction
{
	enum struct Type
	{
		PUSH, POP,
		ADD, SUB, MUL, DIV, MOD,
		AND, OR, XOR, NOT,
		LSHIFT, RSHIFT,
		MOV,
		LABEL, JUMP, IFEQ, CALL,
		PRINT, SYSCALL // Will probably get replaced by some sort of sys_call
	};
	
	Type type;
	
	Instruction(Instruction::Type type = Instruction::Type::PUSH);
	
	static Instruction::Type name_to_type(std::string &name);
	static Instruction::Type name_to_type(const char *name);

	static const char *type_to_name(Instruction::Type type);
};
