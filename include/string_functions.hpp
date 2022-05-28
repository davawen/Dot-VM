#pragma once

#include <string>
#include <optional>
#include <algorithm>
#include <stdexcept>
#include <filesystem>
#include <fmt/format.h>

#include <cstring>

/// Generates a random alpha-numeric string of the given length
std::string random_string(size_t length);

/// Takes a slice from a string, from index a to index b
std::string string_slice(const std::string &self, size_t a, size_t b);

/// Search string 'str' for the string 'query', which is either have the the given delimeters on its side, or be on the borders of the string 
/// @returns the index where query starts if it was found, and std::string::npos otherwise
size_t find_delimited_string(const std::string &str, const std::string &query, const char *delimeters);


/// Iterates over a string and calls a function for every characters in it, but ignores anything wrapped in double quotes
/// @param func function used with signature `int(std::string &, size_t &)`. Non zero return value indicates to break out of loop
template <typename F>
void iterate_ignore_quotes(std::string &str, F func, size_t pos = 0)
{
	static_assert(std::is_invocable<F, std::string &, size_t &>::value, "Invalid function signature passed to iterate_ignore_quotes, must respect int(std::string &, size_t &)");

	bool inQuotes = false;

	while(pos < str.length())
	{
		if(inQuotes)
		{
			if(str[pos] == '\\')
			{
				pos++;
			}
			else if(str[pos] == '\"')
			{
				inQuotes = false;
			}
		}
		else if(str[pos] == '"')
		{
			inQuotes = true;
		}
		else
		{
			int res = func(str, pos);

			if(res != 0) break;
		}

		pos++;
	} 
}

/// Finds a string inside another, ignoring anything inside double quotes
/// @param self string which is searched
/// @param target string to search
/// @param pos position from which to start the search
/// @returns Start index of target string, or std::string::npos if it isn't found
size_t find_ignore_quotes(const std::string &self, const std::string &target, size_t pos = 0);

/// Finds a character inside a given string, ignoring anything inside double quotes
/// @param self string inside which to search
/// @param target Character to search
/// @param pos Position from which to start the search
/// @returns Start index of target character, or std::string::npos if it isn't found
size_t find_ignore_quotes(const std::string &self, char target, size_t pos = 0);

template <> struct fmt::formatter<std::filesystem::path> {
	constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
		auto it = ctx.begin(), end = ctx.end();

		if (it != end && *it != '}') throw format_error("invalid format");

		return it;
	}

	template <typename FormatContext>
	auto format(const std::filesystem::path &path, FormatContext &ctx) -> decltype(ctx.out()) {
		return format_to(ctx.out(), "{}", path.generic_string());
	}
};
