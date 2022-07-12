#pragma once

#include <string_view>

#include "error_handling.hpp"
#include "preproc/line.hpp"
#include "preproc/macro.hpp"
#include "string_functions.hpp"

bool is_preproc_condition(Line &line);
bool is_preproc_endcondition(Line &line);

size_t next_matching_condition(std::vector<Line> &output, size_t idx);
size_t next_matching_endif(std::vector<Line> &output, size_t idx);

std::string_view get_condition_type(Line &line);
