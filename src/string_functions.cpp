#include "string_functions.hpp"

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

std::string string_slice(const std::string &self, size_t a, size_t b)
{
	if(b < a) throw std::invalid_argument("Reverse order slice");
	if(b == a) return std::string("");
	if(b == std::string::npos) return self.substr(a);

	return self.substr(a, b-a);
}

size_t find_delimited_string(const std::string &str, const std::string &query, const char *delimeters)
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

size_t find_ignore_quotes(const std::string &self, const std::string &target, size_t pos)
{
	if(self.empty()) return target.empty() ? 0 : std::string::npos;
	if(target.length() > self.length()) return std::string::npos;

	bool inQuotation = false;

	// At the end of the iteration, you can't find a string if the remaining space inside self is less than target's length
	for(; pos < self.length()-target.length() + 1; pos++)
	{
		if(self[pos] == '"') inQuotation = !inQuotation;
		else if(self[pos] == '\\') pos++; // skip character
		else if(!inQuotation)
		{
			for(size_t i = 0; i < target.length(); i++)
			{
				if(self[pos + i] != target[i]) break;

				if(i == target.length()-1) return pos;
			}
		}
	}

	return std::string::npos;
}

size_t find_ignore_quotes(const std::string &self, char target, size_t pos)
{
	bool inQuotation = false;

	for(; pos < self.length(); pos++)
	{
		if(self[pos] == '"') inQuotation = !inQuotation;
		else if(self[pos] == '\\') pos++; // skip next character
		else if(!inQuotation && self[pos] == target) return pos;
	}

	return std::string::npos;
}
