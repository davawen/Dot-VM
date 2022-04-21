#include <vector>
#include <cstring>

#include "parser/token.hpp"
#include "error_handling.hpp"

/// Takes a file and tokenizes it.
///
/// Outputs the resulting tokens to the 'tokens' vector
void tokenize(const char *filename, std::vector<Token> &tokens);
