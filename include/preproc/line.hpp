#pragma once

#include <string>
#include <filesystem>
#include <cinttypes>

// This is wholly inneficient
struct Line
{
	std::string content;
	std::filesystem::path file;
	size_t line;

	Line(const std::string &str, const std::filesystem::path file, size_t line);
};
