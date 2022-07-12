#pragma once

#include <cstdint>

constexpr intptr_t hash(const char *str) {
	intptr_t hash = 5381;
	char c = *str;

	while ((c = *str++)) hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}
