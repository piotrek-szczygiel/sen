#pragma once

#include <fmt/format.h>

#include <string>
#include <unordered_map>

#include "common.h"
#include "intern.h"

enum Token_Kind {
    TOK_UNKNOWN = 0,

    // Reserve for single character ASCII tokens

    TOK_IDENT = 256,

    TOK_INT,
    TOK_STR,

    TOK_ARROW,
    TOK_DEF_ASSIGN,
};

struct Token {
    std::string str() const { return std::string(begin, end); }

    Token_Kind kind;
    bool valid;

    const byte* begin;
    const byte* end;

    int line_begin, column_begin;
    int line_end, column_end;

    union {
        i64 v_int;
        Interned v_str;
    };
};

template <>
struct fmt::formatter<Token> {
    constexpr auto parse(format_parse_context& ctx) { return ctx.begin(); }

    template <typename FormatContext>
    auto format(const Token& t, FormatContext& ctx) {
        static const std::unordered_map<Token_Kind, std::string> NAMES = {
            {TOK_UNKNOWN, "TOK_UNKNOWN"}, {TOK_IDENT, "TOK_IDENT"},
            {TOK_INT, "TOK_INT"},         {TOK_STR, "TOK_STR"},
            {TOK_ARROW, "TOK_ARROW"},     {TOK_DEF_ASSIGN, "TOK_DEF_ASSIGN"},
        };

        std::string name;
        auto found = NAMES.find(t.kind);
        if (found == NAMES.end()) {
            name = t.kind < 256 ? "TOK" : "UNKNOWN";
        } else {
            name = found->second;
        }

        return format_to(ctx.out(), "{}: '{}'", name, t.str());
    }
};
