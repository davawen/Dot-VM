#include <fmt/format.h>
#include <sys/syscall.h>
#include <unistd.h>

#include <cinttypes>
#include <iostream>
#include <stack>
#include <vector>

#include "hash.hpp"
#include "parser/parse_instruction.hpp"
#include "statement.hpp"
#include "value.hpp"

void interpret(std::vector<Statement> &statements);

void compile_to_c_linux(std::vector<Statement> &statements);
void compile_to_nasm_linux(std::vector<Statement> &statements);
