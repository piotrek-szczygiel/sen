#pragma once

#include "Common.hh"
#include <string>

enum Token_Kind : u8 {
    TOK_EOF = 0,

    // ASCII Chars

    TOK_NAME = 128,
    TOK_INT,
    TOK_FLOAT,
    TOK_IF,
    TOK_FOR,
    TOK_WHILE,
    TOK_FN,

    TOK_INVALID,
    TOK_COUNT,
};

struct Token {
    u8 kind { TOK_INVALID };

    u8* start;
    u8* end;

    union {
        s64 s64_val;
        f64 f64_val;
        bool bool_val;
    };

    Token(u8 kind)
        : kind(kind)
    {
    }

    std::string to_str();
};
