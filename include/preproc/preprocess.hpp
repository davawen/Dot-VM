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
#include "string_functions.hpp"
#include "preproc/line.hpp"
#include "preproc/macro.hpp"

std::vector<Line> preprocess(const char *filename);
