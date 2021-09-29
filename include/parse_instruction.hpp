#pragma once

#include <vector>

#include <cstdio>
#include <cstring>

#include "hash.hpp"
#include "expression.hpp"

Instruction::Type get_instruction_type(const char *str);

/// This iterates over a string and escapes the given sequences
char *handle_escape_sequences(char *str);

void parse_instructions(const char *filename, std::vector<Expression> &expressions);
