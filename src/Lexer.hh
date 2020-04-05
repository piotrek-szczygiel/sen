#pragma once
#include "Common.hh"
#include "Token.hh"
#include <unordered_map>
#include <vector>

struct Lexer {
    void init();
    void free();

    void set_input_from_string(const std::string& string);
    void set_input_from_file(FILE* file);

    u64 intern_string(u8* start, u8* end);

    u8 peek_char();
    u8 peek_char(int lookahead);
    void eat_char();
    void eat_char(int count);
    void eat_whitespace();
    void eat_name();
    s64 eat_int_value();

    Token eat_token();
    void lex();

    void print_info();

    int input_size;
    u8* input;
    u8* current;

    int current_line;
    int current_column;

    int processed_lines;

    std::vector<Token> output;

    std::unordered_map<std::string, u64> intern_map;
    std::vector<std::string> intern_vector;
};
