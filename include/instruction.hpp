#pragma once
#include <string>
#include <unordered_map>
#include <fmt/format.h>

#include "hash.hpp"
#include "error_handling.hpp"

struct Instruction
{
	enum struct Type
	{
		PUSH, POP, SWAP,
		ADD, SUB, MUL, DIV, MOD,
		AND, OR, XOR, NOT,
		LSHIFT, RSHIFT,
		MOV, MOVN,
		LABEL, JUMP, IFEQ, CALL,
		PRINT, SYSCALL, NOP
	};
	
	Type type;
	
	Instruction(Instruction::Type type = Instruction::Type::PUSH);
	
	static Instruction::Type name_to_type(std::string &name);
	static Instruction::Type name_to_type(const char *name);

	static const char *type_to_name(Instruction::Type type);
};
