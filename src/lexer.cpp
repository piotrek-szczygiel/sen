#include "lexer.h"

#include <cassert>
#include <chrono>
#include <cstdarg>
#include <sstream>

#include "common.h"

namespace chrono = std::chrono;

static inline bool valid_ident_start(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b == '_') || (b >= 0xC0));
}

static inline bool valid_ident_continuation(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b >= '0' && b <= '9') ||
            (b == '_') || (b >= 0x80));
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
    current_tok.v_str = intern_string(std::string(current_tok.start, cur));
    current_tok.kind = TOK_IDENT;
}

inline void Lexer::eat_number() {
    u64 value = 0;
    u64 max = INT64_MAX;

    while (valid_number(peek())) {
        u64 digit = eat() - '0';

        if (value > (max - digit) / 10) {
            while (valid_number(peek())) eat();
            auto number = std::string(current_tok.start, cur);
            error("integer is too big: " + number + " > " + std::to_string(max));
            current_tok.kind = TOK_ERR;
            return;
        }

        value = value * 10 + digit;
    }

    current_tok.kind = TOK_I64;
    current_tok.v_i64 = value;
}

inline void Lexer::eat_string() {
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
                    default: error("invalid escape character: \\" + std::string(1, peek())); break;
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
        current_tok.kind = TOK_STR;
        current_tok.v_str = intern_string(ss.str());
    } else {
        current_tok.kind = TOK_ERR;
    }
}

Interned_String Lexer::intern_string(const std::string &str) {
    auto found = interned_map.find(str);
    if (found == interned_map.end()) {
        u64 id = interned_vec.size();
        interned_vec.push_back(str);
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

void Lexer::error(const std::string &msg) {
    current_tok.end = cur;
    errors.push_back({msg, current_tok});
}

void Lexer::print_tokens() {
    for (const auto &tok : tokens) {
        printf("%s", tok.info().c_str());
        if (tok.kind == TOK_IDENT || tok.kind == TOK_STR) {
            printf(" \"%s\"", unintern_string(tok.v_str));
        }
        printf("\n");
    }
}

void Lexer::print_errors() {
    for (const auto &err : errors) {
        if (filename) fprintf(stderr, "%s:", filename);
        fprintf(stderr, "%d:%d: ", err.tok.pos.line, err.tok.pos.column);
        fprintf(stderr, "%s\n", err.msg.c_str());
    }
}

bool Lexer::lex() {
    pos.line = 1;
    pos.column = 1;

    // NOTE eat_string() returns 0 on invalid string
    assert(0 == intern_string("@@@ INVALID STRING @@@"));

    auto t1 = std::chrono::system_clock::now();

    while (peek()) {
        eat_whitespace();

        current_tok.kind = TOK_ERR;
        current_tok.pos = pos;
        current_tok.start = cur;

        switch (peek()) {
            case '\0': {
                eat();
                current_tok.kind = TOK_EOF;
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
                    current_tok.kind = (Token_Kind)eat();
                }
            } break;

            case '-': {
                if (peek(1) == '>') {
                    eat(2);
                    current_tok.kind = TOK_ARROW;
                } else {
                    current_tok.kind = (Token_Kind)eat();
                }
            } break;

            case ':': {
                if (peek(1) == '=') {
                    eat(2);
                    current_tok.kind = TOK_DEF_ASSIGN;
                } else {
                    current_tok.kind = (Token_Kind)eat();
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
                current_tok.kind = (Token_Kind)eat();
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
                eat_number();
            } break;

            case '"': {
                eat();
                eat_string();
            } break;

            default: {
                if (valid_ident_start(peek())) {
                    eat_ident();
                } else {
                    error("unexpected character: " + std::string(1, peek()));
                    eat();
                }
            } break;
        }

        if (current_tok.kind == TOK_EOF) break;

        if (errors.size() > MAX_LEXER_ERRORS) {
            fprintf(stderr, "lexer reached maximum number of errors: %d\n", MAX_LEXER_ERRORS);
            break;
        }

        current_tok.end = cur;
        tokens.push_back(current_tok);
    }

    if (!errors.empty()) return false;

    auto t2 = chrono::system_clock::now();
    auto elapsed = chrono::duration_cast<chrono::microseconds>(t2 - t1);
    auto ms = (f64)elapsed.count() / 1000.0;
    f64 lpms = (f64)pos.line / ms;

    printf("lexed %d lines in %.2fms (%.0f lines/ms)\n", pos.line, ms, lpms);
    return true;
}
