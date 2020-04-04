#include <cstdio>
#include "external/linenoise.hpp"
#include "Lexer.hh"

int main(int argc, char **argv)
{
    linenoise::SetMultiLine(true);
    linenoise::SetHistoryMaxLen(100);
    linenoise::SetCompletionCallback([](const char *editBuffer, std::vector<std::string> &completions) {
        if (editBuffer[0] == 'q')
        {
            completions.push_back("quit");
        }
    });

    while (true)
    {
        std::string line;
        auto quit = linenoise::Readline("> ", line);

        if (quit || line == "quit")
            break;

        Lexer l(line.c_str());
        if (!l.run())
        {
            printf("Lexing error!\n");
        }

        linenoise::AddHistory(line.c_str());
    }
    return 0;
}
