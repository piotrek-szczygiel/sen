#include "lexer.h"
#include "common.h"

#include <sstream>

static inline bool valid_ident_start(byte cc) {
    return ((cc >= 'A' && cc <= 'Z') ||
            (cc >= 'a' && cc <= 'z')); // || (cc >= 0xC0));
}

static inline bool valid_ident_continuation(byte cc) {
    return ((cc >= 'A' && cc <= 'Z') || (cc >= 'a' && cc <= 'z') ||
            (cc >= '0' && cc <= '9')); // || (cc >= 0x80));
}

inline byte Lexer::peek() {
    return *cc;
}

inline byte Lexer::peek(int offset) {
    return *(cc + offset);
}

inline byte Lexer::eat() {
    pos.column++;
    return *cc++;
}

inline void Lexer::eat(int offset) {
    pos.column += offset;
    cc += offset;
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
            byte c = eat();
            if (c + peek() == '\r' + '\n') eat();
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

inline void Lexer::eat_number() {
    while (peek() >= '0' && peek() <= '9') eat();
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
        logger("ate string: '%s'\n", ss.str().c_str());
    } else {
        ss.clear();
    }
}

void Lexer::lex() {
    pos.line = 1;
    pos.column = 1;

    tokens.clear();

    while (peek()) {
        eat_whitespace();

        Token token;
        token.pos = pos;
        token.start = cc;

        switch (peek()) {
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

        case '\0': {
            eat();
            token.kind = TOK_EOF;
        } break;

        case ';':
        case '(':
        case ')':
        case '{':
        case '}':
        case '.':
        case ',':
        case ':':
        case '+': {
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
            eat_number();
            token.kind = TOK_NUMBER;
        } break;

        case '"': {
            eat();
            eat_string();
            token.kind = TOK_STRING;
        } break;

        default: {
            if (valid_ident_start(peek())) {
                eat_ident();
                token.kind = TOK_IDENT;
            } else {
                token.kind = TOK_ERR;
                logger("invalid character: (%d) %c\n", peek(), peek());
                eat();
            }
        } break;
        }

        token.end = cc;
        if (token.kind == TOK_EOF) break;

        tokens.push_back(token);
    }
}
