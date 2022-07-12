#include <preproc/line.hpp>

Line::Line(const std::string &str, const std::filesystem::path file, size_t line) {
	this->content = str;
	this->file = file;
	this->line = line;
}
