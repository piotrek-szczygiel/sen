#include "lexer.h"

#include <fmt/core.h>

#include <chrono>
#include <fstream>
#include <vector>

using fmt::print, fmt::format;
using namespace std::chrono;

static inline bool valid_number(byte b) {
    return (b >= '0' && b <= '9');
}

static inline bool valid_ident_start(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b == '_') || (b >= 0xC0));
}

static inline bool valid_ident_continuation(byte b) {
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b >= '0' && b <= '9') ||
            (b == '_') || (b >= 0x80));
}

bool Lexer::load_file(const std::string& filename_) {
    std::ifstream file(filename_, std::ios::binary);
    if (!file) return false;

    file.seekg(0, std::ios::end);
    size_t length = file.tellg();
    file.seekg(0, std::ios::beg);
    input.resize(length + 1);
    file.read((char*)input.data(), length);

    filename = filename_;

    print("Loaded {} ({} bytes)\n", filename, length);
    return true;
}

bool Lexer::lex() {
    cursor = input.data();

    output.clear();

    intern_table.map.clear();
    intern_table.vec.clear();

    line = 1;
    column = 1;

    errors.clear();
    warnings.clear();

    fill_keywords();

    error_limit = false;

    auto time_begin = system_clock::now();

    bool eof = false;
    while (!eof && !error_limit) {
        eat_whitespace();

        switch (peek()) {
            case '\0': eof = true; break;

            case '/': {
                if (peek(1) == '/') {  // Comment
                    eat(2);
                    while (peek() != '\n' && peek() != '\r') eat();
                } else {
                    eat_token_ascii();
                }
            } break;

            case '-': {
                if (peek(1) == '>') {
                    eat_token_length(TOK_ARROW, 2);
                } else {
                    eat_token_ascii();
                }
            } break;

            case ':': {
                if (peek(1) == '=') {
                    eat_token_length(TOK_DEF_ASSIGN, 2);
                } else {
                    eat_token_ascii();
                }
            } break;

            case '*': {
                if (peek(1) == '*') {
                    eat_token_length(TOK_POW, 2);
                } else {
                    eat_token_ascii();
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
            case '%': eat_token_ascii(); break;

            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': eat_number(); break;

            case '"': eat_string(); break;

            default: {
                if (valid_ident_start(peek())) {
                    eat_ident();
                } else {
                    auto token = begin_token(TOK_UNKNOWN);
                    byte b = eat();
                    error(token, format("Unexpected character: '{}' (ASCII {})", (char)b, (int)b));
                }
            }
        }
    }

    auto time_end = system_clock::now();
    auto elapsed = duration_cast<nanoseconds>(time_end - time_begin);
    auto ms = (f64)elapsed.count() / 1000'000.0;
    f64 lines_per_ms = (f64)line / ms;

    print("Lexed {} lines in {:.1f}ms ({:.1f} lines/ms)\n", line, ms, lines_per_ms);

    for (const auto& e : errors) {
        print("{}:{}:{}: error: {}\n", filename, e.token.line_begin, e.token.column_begin, e.msg);
    }

    for (const auto& w : warnings) {
        print("{}:{}:{}: warning: {}\n", filename, w.token.line_begin, w.token.column_begin, w.msg);
    }

    if (!errors.empty()) {
        if (error_limit) print("Stopped lexing after encountering {} errors\n", errors.size());
        return false;
    }

    return true;
}

void Lexer::fill_keywords() {
    auto& t = intern_table;
    keywords = {
        {t.intern("use"), TOK_USE},
        {t.intern("fn"), TOK_FN},
    };
}

Token Lexer::begin_token(Token_Kind kind) {
    Token token;
    token.kind = kind;
    token.valid = true;
    token.begin = cursor;
    token.line_begin = line;
    token.column_begin = column;
    return token;
}

void Lexer::end_token(Token& token) {
    token.end = cursor;
    token.line_end = line;
    token.column_end = column;
}

inline byte Lexer::peek() {
    return *cursor;
}

inline byte Lexer::peek(int offset) {
    return *(cursor + offset);
}

inline byte Lexer::eat() {
    column++;
    return *cursor++;
}

inline void Lexer::eat(int length) {
    column += length;
    cursor += length;
}

void Lexer::eat_whitespace() {
    while (1) {
        switch (peek()) {
            case ' ':
            case '\t': eat(); break;

            case '\n':
            case '\r': {
                byte b = eat();
                if (b + peek() == '\r' + '\n') eat();
                line++;
                column = 1;
            } break;

            default: return;
        }
    }
}

void Lexer::eat_number() {
    auto token = begin_token(TOK_INT);

    u64 value = 0;
    u64 max = INT64_MAX;

    while (valid_number(peek())) {
        u64 digit = eat() - '0';

        if (value > (max - digit) / 10) {
            while (valid_number(peek())) eat();
            end_token(token);
            token.valid = false;
            emit(token);
            error(token, format("Integer is too big: {}", token.str()));
            return;
        }

        value = value * 10 + digit;
    }

    token.v_int = value;
    end_token(token);
    emit(token);
}

void Lexer::eat_string() {
    auto token = begin_token(TOK_STR);

    eat();  // Eat "

    std::vector<byte> unknown_escapes;

    std::vector<byte> str;
    bool closed = false;
    while (!closed) {
        switch (peek()) {
            case '\\': {
                eat();
                switch (peek()) {
                    case '\\': str.push_back('\\'); break;
                    case '"': str.push_back('"'); break;
                    case 'n': str.push_back('\n'); break;
                    case 't': str.push_back('\t'); break;
                    case '0': str.push_back('\0'); break;
                    case '\0': return;
                    default: unknown_escapes.push_back(peek()); break;
                }
                eat();
            } break;

            case '"': {
                eat();
                closed = true;
            } break;

            case '\0': return;
            default: str.push_back(eat()); break;
        }
    }

    end_token(token);
    emit(token);

    for (char escape : unknown_escapes) {
        warning(token, format("Invalid escape sequence '\\{}'", escape));
    }
}

void Lexer::eat_ident() {
    auto token = begin_token(TOK_IDENT);
    while (valid_ident_continuation(peek())) eat();
    end_token(token);
    auto str = intern_table.intern(token.str());
    auto found = keywords.find(str);
    if (found != keywords.end()) {
        token.kind = found->second;
    } else {
        token.v_str = str;
    }
    emit(token);
}

inline void Lexer::eat_token_ascii() {
    eat_token_length((Token_Kind)peek(), 1);
}

void Lexer::eat_token_length(Token_Kind kind, int length) {
    auto token = begin_token(kind);
    eat(length);
    end_token(token);
    emit(token);
}

void Lexer::emit(Token token) {
    output.push_back(token);
}

void Lexer::error(Token token, const std::string& msg) {
    Lexer_Diagnostic err;
    err.token = token;
    err.msg = msg;
    errors.push_back(err);

    if (errors.size() == MAX_LEXER_ERRORS) error_limit = true;
}

void Lexer::warning(Token token, const std::string& msg) {
    Lexer_Diagnostic warn;
    warn.token = token;
    warn.msg = msg;
    warnings.push_back(warn);
}
