#pragma once
#include "common.h"
#include "token.h"

#include <vector>

struct Lexer {
    Lexer(const byte *input) : input(input), cc(input) {}

    void lex();

    const byte *input;
    const byte *cc;

    File_Position pos;

    std::vector<Token> tokens;

  private:
    byte peek();
    byte peek(int offset);
    byte eat();
    void eat(int offset);

    void eat_whitespace();
    void eat_ident();
    void eat_number();
    void eat_string();
};
