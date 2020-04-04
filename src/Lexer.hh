#pragma once

#include "Token.hh"
#include <vector>

struct Lexer {
    const u8* input;
    std::vector<Token> output;

    u8* cur;

    Lexer(const char* input)
        : input((u8*)input)
        , cur((u8*)input)
    {
    }

    bool run();
};
