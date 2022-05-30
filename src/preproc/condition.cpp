#include "preproc/condition.hpp"

bool is_preproc_condition(Line &line)
{
	return line.content.find("#if") || line.content.find("#elif");
}

bool is_preproc_endcondition(Line &line)
{
	return line.content.find("#endif");
}

size_t next_matching_condition(std::vector<Line> &output, size_t idx)
{
	Line &start_line = output[idx];

	if(!is_preproc_condition(start_line))
	{
		compile_error(start_line, "Can't find matching condition, given line doesn't contain an #if directive");
	}

	size_t scope = 1;

	for(size_t i = idx + 1; i < output.size(); i++)
	{
		Line &line = output[i];

		if(line.content.find("#if")) scope++;
		else if(line.content.find("#elif"))
		{
			if(scope == 1) return i;
		}
		else if(is_preproc_endcondition(output[i])) scope--;

		// Matching #endif
		if(scope == 0) return i;
	}
	
	compile_error(start_line, "Unfinished #if directive, couldn't find matching #elif or #endif");

	return 0;
}

size_t next_matching_endif(std::vector<Line> &output, size_t idx)
{
	Line &start_line = output[idx];

	if(!is_preproc_condition(start_line))
	{
		compile_error(start_line, "Can't find matching condition, given line doesn't contain an #if directive");
	}

	size_t scope = 1;

	for(size_t i = idx + 1; i < output.size(); i++)
	{
		Line &line = output[i];

		if(line.content.find("#if")) scope++;
		else if(is_preproc_endcondition(output[i])) scope--;

		// Matching #endif
		if(scope == 0) return i;
	}
	
	compile_error(start_line, "Unfinished #if directive, couldn't find matching #elif or #endif");

	return 0;
}
