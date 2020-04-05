#pragma once

#include "Common.hh"

enum Token_Kind {
    TOK_EOF = 0,

    // Reserve ASCII characters for single character tokens

    TOK_NAME = 256,
    TOK_INT,
    TOK_FLOAT,
    TOK_IF,
    TOK_FOR,
    TOK_WHILE,
    TOK_FN,

    TOK_ERROR,
};

struct Token {
    Token_Kind kind = TOK_ERROR;

    int l0 = 0;
    int c0 = 0;
    int l1 = -1;
    int c1 = -1;

    union {
        u64 interned_name;

        s64 int_value;
        f64 float_value;

        struct {
            u64 size;
            u8* data;
        } string_value;
    };
};
