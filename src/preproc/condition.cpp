#include "preproc/condition.hpp"

// NOTE: std::string::rfind(..., 0) checks only the start of the string

bool is_preproc_condition(Line &line) { return line.content.rfind("#if", 0) || line.content.rfind("#elif", 0); }

bool is_preproc_endcondition(Line &line) { return line.content.rfind("#endif", 0); }

size_t next_matching_condition(std::vector<Line> &output, size_t idx) {
	Line &start_line = output[idx];

	if (!is_preproc_condition(start_line)) {
		compile_error(start_line, "Can't find matching condition, given line doesn't contain an #if directive");
	}

	size_t scope = 1;

	for (size_t i = idx + 1; i < output.size(); i++) {
		Line &line = output[i];

		if (line.content.rfind("#if", 0))
			scope++;
		else if (line.content.rfind("#elif", 0)) {
			if (scope == 1) return i;
		} else if (is_preproc_endcondition(output[i]))
			scope--;

		// Matching #endif
		if (scope == 0) return i;
	}

	compile_error(start_line, "Unfinished #if directive, couldn't find matching #elif or #endif");

	return 0;
}

size_t next_matching_endif(std::vector<Line> &output, size_t idx) {
	Line &start_line = output[idx];

	if (!is_preproc_condition(start_line)) {
		compile_error(start_line, "Can't find matching condition, given line doesn't contain an #if directive");
	}

	size_t scope = 1;

	for (size_t i = idx + 1; i < output.size(); i++) {
		Line &line = output[i];

		if (line.content.rfind("#if", 0))
			scope++;
		else if (is_preproc_endcondition(output[i]))
			scope--;

		// Matching #endif
		if (scope == 0) return i;
	}

	compile_error(start_line, "Unfinished #if directive, couldn't find matching #elif or #endif");

	return 0;
}

std::string_view get_condition_type(Line &line) {
	// #if<space>LITERAL<space>comparison[<space>LITERAL]

	// #if ___#(MACRO)___ ndef VALUE

	size_t pos = line.content.find_first_of(' ') + 1;
	pos = line.content.find(' ', pos) + 1;
}
