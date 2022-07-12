#include <unistd.h>

#include <cstring>
#include <vector>

#include "error_handling.hpp"
#include "parser/token.hpp"
#include "preproc/line.hpp"
#include "string_functions.hpp"

/// Takes a file and tokenizes it.
std::vector<Token> tokenize(const std::vector<Line> &input);
