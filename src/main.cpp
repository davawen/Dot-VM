#include <algorithm>
#include <exception>
#include <filesystem>
#include <iostream>
#include <list>
#include <ostream>
#include <stdexcept>
#include <vector>

#include <cinttypes>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "instruction.hpp"
#include "statement.hpp"

#include "preproc/line.hpp"
#include "preproc/preprocess.hpp"

#include "parser/parse_instruction.hpp"
#include "parser/tokenize.hpp"

#include "interpreter.hpp"

#define FLAG_IMPLEMENTATION
#include "flags.hpp"
#undef FLAG_IMPLEMENTATION

namespace fs = std::filesystem;

int main(int argc, char **argv) {
  bool *help = flag_bool("help", "Prints this help and exits.", false);

  const char **file = flag_str("file", "Input file.", NULL);
  bool *comp =
      flag_bool("comp", "Wether the program should get compiled. [WIP]", false);

  parse_flags(argc, argv);

  if (*help || argc == 1) {
    print_help("dot-vm --file FILENAME [--comp]",
               "Takes a dotvm file and either interpret it or compiles it to a "
               "binary.");

    return EXIT_SUCCESS;
  }

  if (*file == NULL) {
    printf("Error: no input file recieved.\n");
    return EXIT_FAILURE;
  }

  if (!fs::exists(*file) || !fs::is_regular_file(*file)) {
    printf("Error: incorrect file given.\n");
    return EXIT_FAILURE;
  }

  std::vector<Line> preprocessed = preprocess(*file);

  fmt::print("Preprocessed output:\n");
  // for(auto &str : preprocessed) fmt::print("{}: {} | {}\n", str.file,
  // str.line, str.content);

  std::vector<Token> tokens = tokenize(preprocessed);

  fmt::print("\nTokenized output:\n");
  // for(auto &token : tokens)
  // {
  // 	//if(token.type == Token::NEWLINE) continue;

  // 	fmt::print("Token type: {}, value: \"{}\", line: {}\n", token.type,
  // token.value, token.line->line);
  // }

  // Parse file

  std::vector<Statement> statements = parse_instructions(tokens);

  printf("\n\n");
  std::for_each(statements.begin(), statements.end(),
                [](Statement &ins) { std::cout << ins << '\n'; });
  //

  printf("\n[STARTING EXECUTION FROM HERE]\n\n");
  //
  interpret(statements);

  return 0;
}
