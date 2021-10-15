#pragma once

#include <vector>
#include <string>

#include <cstdio>
#include <cstring>
#include <cinttypes>

#include "hash.hpp"
#include "statement.hpp"
#include "token.hpp"
#include "register.hpp"
#include "error_handling.hpp"

Instruction::Type get_instruction_type(const char *str);

/// This iterates over a string and escapes the given sequences
std::string &handle_escape_sequences(std::string &str);

/// Takes a file and tokenizes it.
///
/// Outputs the resulting tokens to the 'tokens' vector
void tokenize(const char *filename, std::vector<Token> &tokens);

/// Takes a list of tokens and parses it into compilable / interpretable expressions
/// 
/// Outputs the resulting expressions to the 'expressions' vector
void parse_instructions(std::vector<Token> &tokens, std::vector<Statement> &statements);
