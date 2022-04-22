#pragma once

#include <vector>
#include <string>
#include <fmt/format.h>

#include <cstdio>
#include <cstring>
#include <cinttypes>

#include "hash.hpp"
#include "statement.hpp"
#include "token.hpp"
#include "register.hpp"
#include "error_handling.hpp"

/// This iterates over a string and escapes the given sequences
char *handle_escape_sequences(char *str);

/// Takes a list of tokens and parses it into compilable / interpretable expressions
/// 
/// Outputs the resulting expressions to the 'expressions' vector
std::vector<Statement> parse_instructions(std::vector<Token> &tokens);
