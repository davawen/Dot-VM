#include "instruction.hpp"

Instruction::Instruction(Instruction::Type type)
{
	this->type = type;
}

// DONE: Move this to Instruction struct
Instruction::Type Instruction::name_to_type(std::string &name)
{
	return name_to_type(name.c_str());
}

Instruction::Type Instruction::name_to_type(const char *name)
{
	static const std::unordered_map<intptr_t, Instruction::Type> hashTable =
	{
		{ hash("push"), Instruction::Type::PUSH },
		{ hash("pop"),  Instruction::Type::POP },
		{ hash("swap"), Instruction::Type::SWAP },
		{ hash("add"),  Instruction::Type::ADD },
		{ hash("sub"),  Instruction::Type::SUB },
		{ hash("mul"),  Instruction::Type::MUL },
		{ hash("div"),  Instruction::Type::DIV },
		{ hash("mod"),  Instruction::Type::MOD },
		{ hash("and"),  Instruction::Type::AND },
		{ hash("or"),   Instruction::Type::OR },
		{ hash("xor"),  Instruction::Type::XOR },
		{ hash("not"),  Instruction::Type::NOT },
		{ hash("lshift"), Instruction::Type::LSHIFT },
		{ hash("rshift"), Instruction::Type::RSHIFT },
		{ hash("mov"),  Instruction::Type::MOV },
		{ hash("movn"), Instruction::Type::MOVN },
		{ hash(":"),    Instruction::Type::LABEL },
		{ hash("jump"), Instruction::Type::JUMP },
		{ hash("ifeq"), Instruction::Type::IFEQ },
		{ hash("call"), Instruction::Type::CALL },
		{ hash("print"), Instruction::Type::PRINT },
		{ hash("syscall"), Instruction::Type::SYSCALL },
		{ hash("nop"), Instruction::Type::NOP }
	};
	
	intptr_t currentHash = hash(name);

	if(hashTable.find(currentHash) == hashTable.end())
	{
		using namespace std::literals::string_view_literals;

		compile_error(0, "Instruction type does not exist. Got: {}"sv, name); // Program exits here
		return Instruction::Type::AND; // Make compiler happy
	}
	
	return hashTable.at(currentHash);
};

const char *Instruction::type_to_name(Instruction::Type type)
{
	// NOTE: Basically an array
	static const std::unordered_map<Instruction::Type, const char *> hashTable = 
	{
		{ Instruction::Type::PUSH, "push" },
		{ Instruction::Type::POP, "pop" },
		{ Instruction::Type::SWAP, "swap" },
		{ Instruction::Type::ADD, "add" },
		{ Instruction::Type::SUB, "sub" },
		{ Instruction::Type::MUL, "mul" },
		{ Instruction::Type::DIV, "div" },
		{ Instruction::Type::MOD, "mod" },
		{ Instruction::Type::AND, "and" },
		{ Instruction::Type::OR, "or" },
		{ Instruction::Type::XOR, "xor" },
		{ Instruction::Type::NOT, "not" },
		{ Instruction::Type::LSHIFT, "lshift" },
		{ Instruction::Type::RSHIFT, "rshift" },
		{ Instruction::Type::MOV, "mov" },
		{ Instruction::Type::MOVN, "movn" },
		{ Instruction::Type::LABEL, "label" },
		{ Instruction::Type::JUMP, "jump" },
		{ Instruction::Type::IFEQ, "ifeq" },
		{ Instruction::Type::CALL, "call" },
		{ Instruction::Type::PRINT, "print" },
		{ Instruction::Type::SYSCALL, "syscall" },
		{ Instruction::Type::NOP, "nop" }
	};

	if(hashTable.find(type) == hashTable.end())
	{
		using namespace std::literals::string_view_literals;

		compile_error(0, "Instruction type does not exist. Got: {}"sv, static_cast<int>(type));
	}

	return hashTable.at(type);
}
