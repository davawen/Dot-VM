#pragma once

#include <string>

#include "preproc/line.hpp"

struct Token {
	enum Type {
		INSTRUCTION,  // Pretty straigth forward
		LITERAL,      // Registers, Labels
		OPERATOR,     // $, not yet implemented +
		NUMBER,
		STRING,
		NEWLINE  // End of current instruction
	} type;

	std::string value;

	/// Observer pointer to the line this token comes from
	const Line *line;

	Token() {
		this->type = Type::INSTRUCTION;
		this->value = "";
		this->line = nullptr;
	}

	Token(Type type, std::string value, const Line *line) {
		this->type = type;
		this->value = value;
		this->line = line;
		// this->pos = pos;
	}
};
