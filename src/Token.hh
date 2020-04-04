#pragma once

#include "Common.hh"

enum Token_Kind : u8
{
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
    Token_Kind kind { TOK_INVALID };

    union {
        s64 s64_val;
        f64 f64_val;
        bool bool_val;
    };
};
