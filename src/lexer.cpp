#include "lexer.h"
#include "common.h"

#include <chrono>
#include <sstream>

namespace chrono = std::chrono;

static inline bool valid_ident_start(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b == '_') ||
            (b >= 0xC0));
}

static inline bool valid_ident_continuation(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') ||
            (b >= '0' && b <= '9') || (b == '_') || (b >= 0x80));
}

static inline bool valid_number(byte b) {
    return (b >= '0' && b <= '9');
}

inline byte Lexer::peek() {
    return *cur;
}

inline byte Lexer::peek(int offset) {
    return *(cur + offset);
}

inline byte Lexer::eat() {
    pos.column++;
    return *cur++;
}

inline void Lexer::eat(int offset) {
    pos.column += offset;
    cur += offset;
}

inline void Lexer::eat_whitespace() {
    while (true) {
        switch (peek()) {
        case ' ':
        case '\t': {
            eat();
        } break;

        case '\n':
        case '\r': {
            byte b = eat();
            if (b + peek() == '\r' + '\n') eat();
            pos.line++;
            pos.column = 1;
        } break;

        default: return;
        }
    }
}

inline void Lexer::eat_ident() {
    while (valid_ident_continuation(peek())) eat();
}

inline void Lexer::eat_number(Token *token) {
    if (token->kind != TOK_I64) {
        logger("unimplemented number kind\n");
        return;
    }

    u64 value = 0;
    u64 max = INT64_MAX;

    while (valid_number(peek())) {
        u64 digit = eat() - '0';

        if (value > (max - digit) / 10) {
            logger("integer overflow\n");
            while (valid_number(peek())) eat();
            break;
        }

        value = value * 10 + digit;
    }

    token->v_i64 = value;
}

inline Interned_String Lexer::eat_string() {
    std::stringstream ss;

    bool closed = false;
    bool loop = true;
    while (loop) {
        switch (peek()) {
        case '\\': {
            eat();
            switch (peek()) {
            case '\\': ss << '\\'; break;
            case '"': ss << '"'; break;
            case 'n': ss << '\n'; break;
            case 't': ss << '\t'; break;
            case '0': ss << '\0'; break;
            default:
                logger("unknown escape character: (%d) %c\n", peek(), peek());
                break;
            }
            eat();
        } break;

        case '"': {
            eat();
            closed = true;
            loop = false;
        } break;

        case '\0': {
            loop = false;
        } break;

        default: {
            ss << (char)eat();
        } break;
        }
    }

    if (closed) {
        return intern_string(ss.str());
    } else {
        // NOTE id of 0 should be considered invalid string
        return 0;
    }
}

Interned_String Lexer::intern_string(const std::string &str) {
    auto found = interned_map.find(str);
    if (found == interned_map.end()) {
        u64 id = interned_vec.size();
        interned_vec.emplace_back(str);
        interned_map.emplace(str, id);
        return id;
    } else {
        return found->second;
    }
}

const char *Lexer::unintern_string(Interned_String id) {
    if (id >= interned_vec.size()) return nullptr;
    return interned_vec[id].c_str();
}

void Lexer::print_tokens() {
    for (const auto &token : tokens) {
        printf("%s", token.info().c_str());
        if (token.kind == TOK_IDENT || token.kind == TOK_STR) {
            printf(" \"%s\"", unintern_string(token.v_str));
        }
        printf("\n");
    }
}

void Lexer::lex() {
    pos.line = 1;
    pos.column = 1;

    interned_map.clear();
    interned_vec.clear();
    // NOTE eat_string() returns 0 on invalid string
    intern_string("@@@ INVALID STRING @@@");

    tokens.clear();

    auto t1 = std::chrono::system_clock::now();

    while (peek()) {
        eat_whitespace();

        Token token;
        token.pos = pos;
        token.start = cur;

        switch (peek()) {
        case '\0': {
            eat();
            token.kind = TOK_EOF;
        } break;

        case '/': {
            if (peek(1) == '/') {
                eat(2);
                while (auto c = peek()) {
                    if (c == '\n' || c == '\r') break;
                    eat();
                }
                continue;
            } else {
                token.kind = (Token_Kind)eat();
            }
        } break;

        case '-': {
            if (peek(1) == '>') {
                eat(2);
                token.kind = TOK_ARROW;
            } else {
                token.kind = (Token_Kind)eat();
            }
        } break;

        case ':': {
            if (peek(1) == '=') {
                eat(2);
                token.kind = TOK_DEF_ASSIGN;
            } else {
                token.kind = (Token_Kind)eat();
            }
        } break;

        case ';':
        case '(':
        case ')':
        case '{':
        case '}':
        case '.':
        case ',':
        case '+':
        case '*': {
            token.kind = (Token_Kind)eat();
        } break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9': {
            token.kind = TOK_I64;
            eat_number(&token);
        } break;

        case '"': {
            eat();
            token.kind = TOK_STR;
            token.v_str = eat_string();
        } break;

        default: {
            if (valid_ident_start(peek())) {
                eat_ident();
                token.v_str = intern_string(std::string(token.start, cur));
                token.kind = TOK_IDENT;
            } else {
                token.kind = TOK_ERR;
                logger("invalid character: (%d) %c\n", peek(), peek());
                eat();
            }
        } break;
        }

        token.end = cur;
        if (token.kind == TOK_EOF) break;

        tokens.push_back(token);
    }

    auto t2 = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    auto ms = (f64)elapsed.count() / 1000.0;
    f64 lpms = (f64)pos.line / ms;

    logger("lexed %d lines in %.2fms (%.0f lines/ms)\n", pos.line, ms, lpms);
}
