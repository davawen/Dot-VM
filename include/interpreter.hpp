#include <iostream>
#include <vector>
#include <stack>
#include <fmt/format.h>

#include <sys/syscall.h>
#include <unistd.h>
#include <cinttypes>

#include "parser/parse_instruction.hpp"
#include "value.hpp"
#include "statement.hpp"
#include "hash.hpp"

void interpret(std::vector<Statement> &statements);

void compile_to_c_linux(std::vector<Statement> &statements);
void compile_to_nasm_linux(std::vector<Statement> &statements);
