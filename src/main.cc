#include <fmt/core.h>

#include "lexer.h"

using fmt::print;

int main(int argc, char** argv) {
    if (argc != 2) {
        print("Usage: sen <file>\n");
        return 2;
    }

    const char* filename = argv[1];

    Lexer lexer;
    if (!lexer.load_file(filename)) {
        print("Unable to load {}\n", filename);
        return 1;
    }

    if (lexer.lex()) {
        for (const auto& t : lexer.output) {
            print("{}\n", t);
        }
    }

    return 0;
}
