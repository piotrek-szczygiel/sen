#pragma once

#include "Common.hh"
#include <string>
#include <unordered_map>
#include <vector>

enum Token_Kind {
    TOK_EOF = 0,

    // Reserve ASCII characters for single character tokens

    TOK_ID = 256,
    TOK_INT,
    TOK_FLOAT,
    TOK_STRING,

    // Keywords
    TOK_KEY_INT,
    TOK_KEY_FLOAT,
    TOK_KEY_STRING,
    TOK_KEY_BOOL,
    TOK_KEY_TRUE,
    TOK_KEY_FALSE,

    TOK_KEY_FN,
    TOK_KEY_IF,
    TOK_KEY_ELSE,
    TOK_KEY_RETURN,

    // Digraphs
    TOK_ARROW,

    TOK_ERROR,
};

struct Token {
    Token_Kind kind = TOK_ERROR;

    int l0 = 0;
    int c0 = 0;
    int l1 = -1;
    int c1 = -1;

    union {
        u64 id;

        s64 int_value;
        f64 float_value;
        u64 string_value;
    };
};
