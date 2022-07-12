#pragma once

#include <cassert>
#include <cinttypes>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <stdexcept>
#include <string>

namespace {
struct Flag {
	enum Type { BOOL, INT64, UINT64, STR };

	Type type;

	const char *name;
	const char *desc;

	uintptr_t def;
	uintptr_t val;

	Flag(void);
};
};  // namespace

static uintptr_t *flag_create(Flag::Type type, const char *name, const char *desc, uintptr_t def);

bool *flag_bool(const char *name, const char *desc, bool def);
int64_t *flag_int64(const char *name, const char *desc, int64_t def);
uint64_t *flag_uint64(const char *name, const char *desc, uint64_t def);
const char **flag_str(const char *name, const char *desc, const char *def);

void parse_flags(int argc, char **argv);

void print_help(const char *programName, const char *programDesc);

#ifdef FLAG_IMPLEMENTATION

#  ifndef FLAG_NUMBER
#	define FLAG_NUMBER 32
#  endif

Flag::Flag(void) {
	this->type = Flag::BOOL;
	this->name = NULL;
	this->desc = NULL;

	this->def = 0;
	this->val = 0;
}

static Flag flags[FLAG_NUMBER];
static int flagsCount = 0;

uintptr_t *flag_create(Flag::Type type, const char *name, const char *desc, uintptr_t def) {
	assert(flagsCount < FLAG_NUMBER);

	Flag &flag = flags[flagsCount++];

	flag.type = type;
	flag.name = name;
	flag.desc = desc;

	flag.def = def;
	flag.val = def;

	return &flag.val;
}

bool *flag_bool(const char *name, const char *desc, bool def) {
	return reinterpret_cast<bool *>(flag_create(Flag::BOOL, name, desc, *reinterpret_cast<uintptr_t *>(&def)));
}

int64_t *flag_int64(const char *name, const char *desc, int64_t def) {
	return reinterpret_cast<int64_t *>(flag_create(Flag::INT64, name, desc, *reinterpret_cast<uintptr_t *>(&def)));
}

uint64_t *flag_uint64(const char *name, const char *desc, uint64_t def) {
	return reinterpret_cast<uint64_t *>(flag_create(Flag::UINT64, name, desc, *reinterpret_cast<uintptr_t *>(&def)));
}

const char **flag_str(const char *name, const char *desc, const char *def) {
	return reinterpret_cast<const char **>(flag_create(Flag::STR, name, desc, reinterpret_cast<uintptr_t>(def)));
}

void parse_flags(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		char *val = argv[i];

		if (val[0] != '-' || val[1] != '-') continue;

		Flag *match = NULL;
		for (int j = 0; j < flagsCount; j++) {
			if (!strcmp(val + 2, flags[j].name)) {
				match = &flags[j];
				break;
			}
		}

		if (match == NULL) continue;

		switch (match->type) {
		case Flag::BOOL:
			match->val = (uintptr_t) true;
			break;
		case Flag::UINT64:
		case Flag::INT64:
			if (++i >= argc) {
				throw std::runtime_error(std::string("Error: Expected argument at ") + match->name + ".\n");
			}

			val = argv[i];

			match->val = match->type == Flag::INT64 ? strtoll(val, NULL, 0) : strtoull(val, NULL, 0);
			break;
		case Flag::STR:
			if (++i >= argc) {
				throw std::runtime_error(std::string("Error: Expected argument at ") + match->name + ".\n");
			}

			match->val = reinterpret_cast<uintptr_t>(argv[i]);
			break;
		}
	}
}

void print_help(const char *programName, const char *programDesc) {
	printf("%s:\n  %s\n\nARGUMENTS:\n\n", programName, programDesc);

	for (int i = 0; i < flagsCount; i++) {
		printf("--%s:\n    %s\n\n", flags[i].name, flags[i].desc);

		switch (flags[i].type) {
		case Flag::BOOL:
			printf("    Type: Boolean\n\n");
			break;
		case Flag::INT64:
			printf("    Type: Int\n    Default value: %" PRId64 "\n\n", *reinterpret_cast<int64_t *>(&flags[i].def));
			break;
		case Flag::UINT64:
			printf("    Type: Unsigned Int\n    Default value: %" PRIu64 "\n\n",
			       *reinterpret_cast<uint64_t *>(&flags[i].def));
			break;
		case Flag::STR:
			if (reinterpret_cast<const char *>(flags[i].def) == NULL) break;

			printf("    Type: String\n    Default value: %s\n\n", reinterpret_cast<const char *>(flags[i].def));
		}
	}
}

#endif
