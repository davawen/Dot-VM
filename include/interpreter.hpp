#include <iostream>
#include <vector>
#include <cinttypes>

#include "value.hpp"
#include "expression.hpp"
#include "parse_instruction.hpp"
#include "hash.hpp"

void interpret_linux(std::vector<Expression> &expressions);

void compile_to_c_linux(std::vector<Expression> &expressions);
void compile_to_nasm_linux(std::vector<Expression> &expressions);