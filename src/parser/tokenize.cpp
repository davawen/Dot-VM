#include "parser/tokenize.hpp"

std::vector<Token> tokenize(const std::vector<Line> &input) {
	std::vector<Token> tokens;

	for (auto &line : input) {
		if (line.content.empty()) continue;

		// valid lines:
		// <empty>
		// instruction_name|
		// instruction_name arg|
		// instruction_name arg1,...,argn|

		size_t tokenIndex = line.content.find(' ');

		std::string instructionName = line.content.substr(0, tokenIndex);
		tokens.push_back(Token(Token::INSTRUCTION, instructionName, &line));

		if (tokenIndex == std::string::npos)  // No arguments given
		{
			tokens.push_back(Token(Token::NEWLINE, "\n", &line));
			continue;
		}

		size_t argIndex = find_ignore_quotes(line.content, ',', tokenIndex);
		// if(argIndex != std::string::npos) fmt::print("\nFound ',' here:\n{}\n{:>{}}\n", line.content, '^', argIndex +
		// 1);

		// Token index is pointing to a space, this makes it point to the start of the first argument
		tokenIndex++;

		while (tokenIndex < line.content.length()) {
			auto argStr = string_slice(line.content, tokenIndex, argIndex);

			Token::Type tokenType;

			while (argStr[0] == '$') {
				tokens.push_back(Token(Token::OPERATOR, "$", &line));

				tokenIndex++;
				argStr = argStr.substr(1);  // Remove first character from string
			}

			// DONE: Move escape sequences to parsing
			if (argStr[0] == '\"')  // if it has a string
			{
				if (argStr.back() != '\"')
					compile_error(line, fmt::format("Unfinished string at argument: {}", argStr));

				argStr = string_slice(argStr, 1, argStr.length() - 1);  // Remove the quotes at either end

				tokenType = Token::STRING;
			} else  // DONE: Differentiate numbers from literals
			{
				auto argCStr = argStr.c_str();
				char *endPtr;

				// TODO: add overflow check
				strtoll(argCStr, &endPtr, 0);

				// From `man strtol`
				// If endptr is not NULL, strtol() stores the address of the first invalid character in *endptr.  If
				// there were no digits at all, strtol() stores the original value of nptr in *endptr (and returns 0).
				// In particular, if *nptr is not '\0' but **endptr is '\0' on return, the entire string is valid.
				bool isNum = endPtr != argCStr && *endPtr == '\0';

				if (isNum)  // Number
				{
					tokenType = Token::NUMBER;
				} else  // Label or register
				{
					tokenType = Token::LITERAL;
				}
			}

			tokens.push_back(Token(tokenType, argStr, &line));

			tokenIndex = argIndex == std::string::npos ? argIndex : argIndex + 1;
			argIndex = find_ignore_quotes(line.content, ',', tokenIndex);
		}
		// while(argIndex != std::string::npos);

		tokens.push_back(Token(Token::NEWLINE, "\n", &line));
	}

	return tokens;
}
