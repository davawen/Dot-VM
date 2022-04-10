#pragma once

#include <iostream>
#include <vector>
#include <stack>
#include <string>
#include <fstream>
#include <functional>
#include <unordered_set>
#include <unordered_map>
#include <cstring>

#include "error_handling.hpp"
#include "preproc/line.hpp"
#include "preproc/macro.hpp"
#include "preproc/string_functions.hpp"

void preprocess(const char *filename, std::vector<Line> &output);
