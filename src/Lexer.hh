#pragma once

#include <vector>
#include "Token.hh"

struct Lexer
{
    const char *input;
    std::vector<Token> output;

    Lexer(const char *input)
        : input(input), output{}
    {
    }

    bool run();
};
