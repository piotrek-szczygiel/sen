#pragma once

#include <unordered_map>
#include <vector>

#include "common.h"
#include "token.h"

// Lexer will terminate after reaching this number of errors
constexpr int MAX_LEXER_ERRORS = 20;

struct Lexer_Error {
    std::string msg;
    Token tok;
};

struct Lexer {
    Lexer(const byte *input) : input(input), cur(input), filename(nullptr) {}
    Lexer(const byte *input, const char *filename) : input(input), cur(input), filename(filename) {}

    bool lex();

    byte peek();
    byte peek(int offset);
    byte eat();
    void eat(int offset);

    void eat_whitespace();
    void eat_ident();
    void eat_number();
    void eat_string();

    Interned_String intern_string(const std::string &str);
    const char *unintern_string(Interned_String id);

    void error(const std::string &msg);
    void print_tokens();
    void print_errors();

    const byte *input;
    const byte *cur;

    const char *filename;

    Token current_tok;

    File_Position pos;

    std::unordered_map<std::string, Interned_String> interned_map;
    std::vector<std::string> interned_vec;

    std::vector<Token> tokens;

    std::vector<Lexer_Error> errors;
};
