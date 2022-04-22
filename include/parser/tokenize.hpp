#include <vector>

#include <cstring>
#include <unistd.h>

#include "preproc/line.hpp"
#include "parser/token.hpp"

#include "error_handling.hpp"
#include "string_functions.hpp"

/// Takes a file and tokenizes it.
std::vector<Token> tokenize(const std::vector<Line> &input);
