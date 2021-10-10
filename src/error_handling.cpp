#include "error_handling.hpp"

void compile_warning(int line, int pos, const char *format, ...)
{
	printf("\x1b[33mWarning on line %i at position %i:\x1b[0m\n", line, pos);
	
	va_list argptr;
	va_start(argptr, format);
	
	vprintf(format, argptr);

	va_end(argptr);

	puts("\n\n");
}

void compile_error(int line, int pos, const char *format, ...)
{
	printf("\n\x1b[101mCompile Error\x1b[49m\x1b[31m on line %i at position %i:\x1b[0m\n", line, pos);
	
	va_list argptr;
	va_start(argptr, format);
	
	vprintf(format, argptr);

	va_end(argptr);
	
	puts("\n\n");

	exit(-1);
}

void runtime_error(const char *format, ...)
{
	printf("\n\x1b[41mFatal error:\x1b[0m\n");
	
	va_list argptr;
	va_start(argptr, format);
	
	vprintf(format, argptr);

	va_end(argptr);
	
	puts("\n\n");

	exit(-1);
}

void runtime_warning(const char *format, ...)
{
	printf("\n\x1b[43mWarning:\x1b[0m\n");
	
	va_list argptr;
	va_start(argptr, format);
	
	vprintf(format, argptr);

	va_end(argptr);
	
	puts("\n\n");

	exit(-1);
}
