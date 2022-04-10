#pragma once

#include <algorithm>
#include <string>
#include <cstring>

/// Generates a random alpha-numeric string of the given length
std::string random_string(size_t length);

/// Search string 'str' for the string 'query', which is either have the the given delimeters on its side, or be on the borders of the string 
/// @returns the index where query starts if it was found, and std::string::npos otherwise
size_t find_string(const std::string &str, const std::string &query, const char *delimeters);
