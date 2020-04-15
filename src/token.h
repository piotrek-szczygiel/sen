#pragma once
#include <string>

#include "common.h"

enum Token_Kind {
    TOK_ERR = 0,

    // Reserve for single character ASCII tokens

    TOK_IDENT = 256,
    TOK_NUMBER,
    TOK_STRING,
    TOK_ARROW,
    TOK_EOF,
};

struct File_Position {
    int line;
    int column;
};

struct Token {
    Token_Kind kind;
    File_Position pos;
    const byte *start;
    const byte *end;

    std::string info() const;
};
