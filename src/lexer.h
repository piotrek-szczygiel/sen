#pragma once
#include "common.h"
#include "token.h"

#include <unordered_map>
#include <vector>

struct Lexer {
    Lexer(const byte *input) : input(input), cc(input) {}

    void lex();

    byte peek();
    byte peek(int offset);
    byte eat();
    void eat(int offset);

    void eat_whitespace();
    void eat_ident();
    u64 eat_number(Token *token, u64 max);
    Interned_String eat_string();

    Interned_String intern_string(const std::string &str);
    const char *unintern_string(Interned_String id);

    void print_tokens();

    const byte *input;
    const byte *cc;

    File_Position pos;

    std::unordered_map<std::string, Interned_String> interned_map;
    std::vector<std::string> interned_vec;

    std::vector<Token> tokens;
};
