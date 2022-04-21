#include "error_handling.hpp"

void compile_warning(int line, const std::string &output)
{
	fmt::print("\n\x1b[43merror:\x1b[0m on line {}:\n", line);
	fmt::print("{}\n", output);
}

void compile_warning(const Line &line, const std::string &output)
{
	fmt::print("\n\x1b[43m warning:\x1b[0m in file {}:\n{:>3} | {}\n", line.file, line.line, line.content);
	fmt::print("{}\n", output);
}

void compile_error(int line, const std::string &output)
{
	fmt::print("\n\x1b[41merror:\x1b[0m on line {}:\n", line);
	fmt::print("{}\n", output);

	throw std::exception();
}

void compile_error(const Line &line, const std::string &output)
{
	fmt::print("\n\x1b[41merror:\x1b[0m in file {}:\n{:>3} | {}\n", line.file, line.line, line.content);
	fmt::print("{}\n", output);

	throw std::exception();
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
