#pragma once

#include <stdexcept>

#include <cstdio>
#include <cstdlib>
#include <cstdarg>

void compile_warning(int line, int pos, const char *format, ...);
void compile_error(int line, int pos, const char *format, ...);

void runtime_warning(const char *format, ...);
void runtime_error(const char *format, ...);
