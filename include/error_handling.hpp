#pragma once

#include <stdexcept>

#include <cstdio>
#include <cstdarg>

#include <fmt/format.h>
#include <fmt/color.h>

#include "string_functions.hpp"
#include "preproc/line.hpp"

#ifndef FMT_TRACE
#define FMT_TRACE(fmtstring, ...) \
	do { \
		fmt::print("{}({}): ", __FILE__, __LINE__); \
		fmt::print(fmtstring, __VA_ARGS__); \
	} while(0)
#endif
	
// TODO: Throw a custom exception instead of this

void compile_warning(int line, const std::string &output);
void compile_warning(const Line &line, const std::string &output);

void compile_error(int line, const std::string &output);
void compile_error(const Line &line, const std::string &output);

void runtime_warning(const char *format, ...);
void runtime_error(const char *format, ...);

// void compile_warning(int line, const char *format, ...)
// {
// 	printf("\x1b[33mWarning on line %i:\x1b[0m\n", line);
// 	
// 	va_list argptr;
// 	va_start(argptr, format);
// 	
// 	vprintf(format, argptr);
// 
// 	va_end(argptr);
// 
// 	puts("\n\n");
// }
// 
// void compile_error(int line, const char *format, ...)
// {
// 	printf("\n\x1b[101mCompile Error\x1b[49m\x1b[31m on line %i:\x1b[0m\n", line);
// 	
// 	va_list argptr;
// 	va_start(argptr, format);
// 	
// 	vprintf(format, argptr);
// 
// 	va_end(argptr);
// 	
// 	puts("\n\n");
// 
// 	exit(-1);
// }

