#pragma once

#include <string>
#include <cinttypes>

// This is wholly inneficient
struct Line
{
	std::string content;
	std::string file;
	size_t line;

	Line(const std::string &str, const char *file, size_t line);
	Line(const std::string &str, const std::string &file, size_t line);
};
