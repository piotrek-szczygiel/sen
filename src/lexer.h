#pragma once

#include <unordered_map>
#include <vector>

#include "common.h"
#include "intern.h"
#include "token.h"

constexpr int MAX_LEXER_ERRORS = 20;

struct Lexer_Diagnostic {
    std::string msg;
    Token token;
};

struct Lexer {
    bool load_file(const std::string& filename);
    bool lex();

    void fill_keywords();

    Token begin_token(Token_Kind kind);
    void end_token(Token& token);

    byte peek();
    byte peek(int offset);

    byte eat();
    void eat(int length);

    void eat_whitespace();
    void eat_number();
    void eat_string();
    void eat_ident();
    void eat_token_ascii();
    void eat_token_length(Token_Kind, int length);

    void emit(Token token);

    void error(Token token, const std::string& msg);
    void warning(Token token, const std::string& msg);

    std::string filename;

    std::vector<byte> input;
    std::vector<Token> output;

    Intern_Table intern_table;

    byte* cursor;

    int line;
    int column;

    std::unordered_map<Interned, Token_Kind> keywords;

    std::vector<Lexer_Diagnostic> errors;
    std::vector<Lexer_Diagnostic> warnings;

    bool error_limit;
};
