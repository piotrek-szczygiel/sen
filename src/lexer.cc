#include "lexer.h"

#include <fmt/core.h>

#include <chrono>
#include <fstream>
#include <vector>

using fmt::print, fmt::format;
using namespace std::chrono;

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
            case 0: eof = true; break;

            case '/': match(1, '/') ? discard_line() : eat_one(); break;

            case '-': match(1, '>') ? eat_two(TOKEN_ARROW) : eat_one(); break;
            case ':': match(1, '=') ? eat_two(TOKEN_DEF_ASSIGN) : eat_one(); break;
            case '*': match(1, '*') ? eat_two(TOKEN_POW) : eat_one(); break;

            case ';':
            case '(':
            case ')':
            case '{':
            case '}':
            case '.':
            case ',':
            case '+':
            case '%': eat_one(); break;

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
                if (is_ident_start()) {
                    eat_ident();
                } else {
                    auto token = begin_token(TOKEN_UNKNOWN);
                    token.valid = false;
                    byte b = advance();
                    end_token(token);
                    emit(token);
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
        {t.intern("use"), TOKEN_USE},
        {t.intern("fn"), TOKEN_FN},
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

inline byte Lexer::advance() {
    column++;
    return *cursor++;
}

inline void Lexer::advance(int length) {
    column += length;
    cursor += length;
}

inline bool Lexer::match(byte expected) {
    return peek() == expected;
}

inline bool Lexer::match(int offset, byte expected) {
    return peek(offset) == expected;
}

inline bool Lexer::is_digit() {
    byte b = peek();
    return (b >= '0' && b <= '9');
}

inline bool Lexer::is_ident_start() {
    byte b = peek();
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b == '_') || (b >= 0xC0));
}

inline bool Lexer::is_ident_continue() {
    byte b = peek();
    return ((b >= 'A' && b <= 'Z') || (b >= 'a' && b <= 'z') || (b >= '0' && b <= '9') ||
            (b == '_') || (b >= 0x80));
}

inline void Lexer::discard_line() {
    while (!match('\n') && !match('\r')) advance();
}

void Lexer::eat_whitespace() {
    while (1) {
        switch (peek()) {
            case ' ':
            case '\t': advance(); break;

            case '\n':
            case '\r': {
                byte b = advance();
                if (b + peek() == '\r' + '\n') advance();
                line++;
                column = 1;
            } break;

            default: return;
        }
    }
}

void Lexer::eat_number() {
    auto token = begin_token(TOKEN_INT);

    u64 value = 0;
    u64 max = INT64_MAX;

    while (is_digit()) {
        u64 digit = advance() - '0';

        if (value > (max - digit) / 10) {
            while (is_digit()) advance();
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
    auto token = begin_token(TOKEN_STR);

    advance();  // Eat "

    std::vector<byte> unknown_escapes;

    std::vector<byte> str;
    bool closed = false;
    while (!closed) {
        switch (peek()) {
            case '\\': {
                advance();
                switch (peek()) {
                    case '\\': str.push_back('\\'); break;
                    case '"': str.push_back('"'); break;
                    case 'n': str.push_back('\n'); break;
                    case 't': str.push_back('\t'); break;
                    case '0': str.push_back(0); break;
                    case 0: return;
                    default: unknown_escapes.push_back(peek()); break;
                }
                advance();
            } break;

            case '"': {
                advance();
                closed = true;
            } break;

            case '\n':
            case '\r': {
                byte b = advance();
                if (b + peek() == '\r' + '\n') advance();
                line++;
                column = 1;

                str.push_back('\n');
            } break;

            case 0: return;
            default: str.push_back(advance()); break;
        }
    }

    end_token(token);
    emit(token);

    for (char escape : unknown_escapes) {
        warning(token, format("Invalid escape sequence '\\{}'", escape));
    }
}

void Lexer::eat_ident() {
    auto token = begin_token(TOKEN_IDENT);
    while (is_ident_continue()) advance();
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

void Lexer::eat_one() {
    auto token = begin_token((Token_Kind)peek());
    advance();
    end_token(token);
    emit(token);
}

void Lexer::eat_two(Token_Kind kind) {
    auto token = begin_token(kind);
    advance(2);
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
