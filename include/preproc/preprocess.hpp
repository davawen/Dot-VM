#pragma once

#include <cstring>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <stack>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "error_handling.hpp"
#include "preproc/condition.hpp"
#include "preproc/line.hpp"
#include "preproc/macro.hpp"
#include "string_functions.hpp"

std::vector<Line> preprocess(const std::filesystem::path filename);
