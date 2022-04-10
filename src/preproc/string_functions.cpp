#include "preproc/string_functions.hpp"

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

size_t find_string(const std::string &str, const std::string &query, const char *delimeters)
{
	size_t i = 0;
	while((i = str.find(query, i)) != std::string::npos)
	{
		if((i == 0 || strchr(delimeters, str[i - 1])) && ((i + query.length()) >= str.length() || strchr(delimeters, str[i + query.length()])))
		{
			return i;
		}
		else
		{
			i++;
		}
	}

	return std::string::npos;
}
