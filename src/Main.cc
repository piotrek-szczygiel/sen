#include "Lexer.hh"
#include "external/linenoise.hpp"
#include <cstdio>

int main(int argc, char** argv)
{
    if (argc == 1) {
        linenoise::SetMultiLine(true);
        linenoise::SetHistoryMaxLen(100);
        linenoise::SetCompletionCallback(
            [](const char* editBuffer, std::vector<std::string>& completions) {
                if (editBuffer[0] == 'q') {
                    completions.push_back("quit");
                }
            });

        Lexer lexer;
        while (true) {
            std::string line;
            bool quit = linenoise::Readline("> ", line);

            if (quit || line == "quit") {
                break;
            }

            lexer.init();
            lexer.set_input_from_string(line);
            lexer.lex();
            lexer.free();

            linenoise::AddHistory(line.c_str());
        }
    } else if (argc == 2) {
        const char* filename = argv[1];
        FILE* file = fopen(filename, "rb");
        if (file == nullptr) {
            printf("unable to open file: %s\n", filename);
            return 1;
        } else {
            Lexer lexer;
            lexer.init();
            lexer.set_input_from_file(file);
            lexer.lex();
            lexer.free();
            fclose(file);
        }
    }
    return 0;
}
