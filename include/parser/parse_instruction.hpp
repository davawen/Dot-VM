#pragma once

#include <fmt/format.h>

#include <cinttypes>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

#include "error_handling.hpp"
#include "hash.hpp"
#include "register.hpp"
#include "statement.hpp"
#include "token.hpp"

/// This iterates over a string and escapes the given sequences
char *handle_escape_sequences(char *str);

/// Takes a list of tokens and parses it into compilable / interpretable expressions
///
/// Outputs the resulting expressions to the 'expressions' vector
std::vector<Statement> parse_instructions(std::vector<Token> &tokens);
