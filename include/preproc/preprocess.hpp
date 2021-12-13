#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <cstring>

#include <error_handling.hpp>
#include <preproc/line.hpp>
#include <preproc/macro.hpp>

void search_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const MacroMap &macros);

void expand_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const size_t pos, const MacroMap &macros, const Macro &macro);
void expand_macro(std::vector<Line> &output, std::vector<Line>::iterator it, const size_t pos, const MacroMap &macros, const Macro &macro, const MacroMap &args);

void preprocess(const char *filename, std::vector<Line> &output);
