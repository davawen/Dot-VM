#include <preproc/line.hpp>

Line::Line(const std::string &str, const char *file, size_t line)
{
	this->content = str;
	this->file = std::string(file);
	this->line = line;
}

Line::Line(const std::string &str, const std::string &file, size_t line)
{
	this->content = str;
	this->file = file;
	this->line = line;
}
