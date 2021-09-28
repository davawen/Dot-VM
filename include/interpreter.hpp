#include <iostream>
#include <vector>
#include <cinttypes>

#include "value.hpp"
#include "instruction.hpp"
#include "parseInstruction.hpp"
#include "hash.hpp"

void interpret(std::vector<Instruction> &instructions);
