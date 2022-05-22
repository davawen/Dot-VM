#pragma once

#include <vector>
#include <string>
#include <string_view>
#include <unordered_map>
#include <fmt/core.h>

#include "error_handling.hpp"
#include "string_functions.hpp"
#include "preproc/line.hpp"

typedef std::vector<std::string> Macro;
typedef std::unordered_map<std::string, Macro> MacroMap;

void search_macro(std::vector<Line> &output, size_t idx, const MacroMap &macros);

// void expand_macro(std::vector<Line> &output, size_t idx, const size_t pos, const Macro &macro);
void expand_macro(std::vector<Line> &output, size_t idx, const size_t pos, const Macro &macro, const MacroMap &args);
