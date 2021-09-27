#pragma once

#include <vector>

#include <cstdio>
#include <cstring>

#include "hash.hpp"
#include "instruction.hpp"
#include "value.hpp"

Instruction::Type get_instruction_type(const char *str);

void parse_instructions(const char *filename, std::vector<Instruction> &instructions);