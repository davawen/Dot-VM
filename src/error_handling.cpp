#include "error_handling.hpp"

void compile_warning(int line, int pos, const char *format, ...)
{
	printf("\x1b[103mWarning\x1b[0m on line %i at %i:\n", line, pos);
	
	va_list argptr;
	va_start(argptr, format);
	
	printf(format, argptr);

	va_end(argptr);
}

void compile_error(int line, int pos, const char *format, ...)
{
	printf("\x1b[101mCompile Error\x1b[0m on line %i at %i:\n", line, pos);
	
	va_list argptr;
	va_start(argptr, format);
	
	printf(format, argptr);

	va_end(argptr);
	
	puts("\n\n");

	exit(-1);
}
