#pragma once

#include <string>

#include "common.h"

enum Token_Kind {
    TOK_ERR = 0,

    // Reserve for single character ASCII tokens

    TOK_IDENT = 256,
    TOK_I64,
    TOK_STR,
    TOK_ARROW,
    TOK_DEF_ASSIGN,
    TOK_EOF,
};

struct File_Position {
    int line;
    int column;
};

struct Token {
    std::string info() const;

    Token_Kind kind;
    File_Position pos;
    const byte *start;
    const byte *end;

    union {
        i64 v_i64;
        f64 v_f64;
        bool v_bool;
        Interned_String v_str;
    };
};
