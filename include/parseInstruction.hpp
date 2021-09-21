#pragma once

#include <vector>

#include <cstdio>
#include <cstring>

#include "hash.hpp"
#include "instruction.hpp"

Instruction::Type getInstruction(const char *str);

void parseInstructions(const char *filename, std::vector<Instruction> &instructions);