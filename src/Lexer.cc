#include "Lexer.hh"
#include "Token.hh"

void Lexer::init()
{
    input_size = -1;
    input = nullptr;
    current = nullptr;
    current_line = 1;
    current_column = 1;
    processed_lines = 1;
    output.clear();
    intern_map.clear();
    intern_vector.clear();
}

void Lexer::free()
{
    if (input != nullptr) {
        delete[] input;
    }
}

void Lexer::set_input_from_string(const std::string& string)
{
    input_size = string.size();
    input = new u8[input_size + 1];
    u8* data = (u8*)string.c_str();
    for (u64 i = 0; i < input_size; ++i) {
        input[i] = data[i];
    }
    input[input_size] = 0;
    current = input;
}

void Lexer::set_input_from_file(FILE* file)
{
    fseek(file, 0, SEEK_END);
    input_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    input = new u8[input_size + 1];
    fread(input, 1, input_size, file);
    input[input_size] = 0;
    current = input;
}

u64 Lexer::intern_string(u8* start, u8* end)
{
    std::string str((char*)start, (size_t)(end - start));
    auto found = intern_map.find(str);
    if (found == intern_map.end()) {
        u64 intern = intern_vector.size();
        intern_vector.emplace_back(str);
        intern_map.emplace(str, intern);
        return intern;
    } else {
        return found->second;
    }
}

inline void Lexer::eat_char()
{
    u8 c = *current++;
    if (c == '\n') {
        ++current_line;
        ++processed_lines;
        current_column = 0;
    } else {
        ++current_column;
    }
}

inline void Lexer::eat_char(int count)
{
    for (int i = 0; i < count; ++i) {
        eat_char();
    }
}

inline u8 Lexer::peek_char()
{
    return *current;
}

inline u8 Lexer::peek_char(int lookahead)
{
    return *(current + lookahead);
}

inline bool is_whitespace(u8 c)
{
    return c == ' ' || c == '\n' || c == '\r' || c == '\t' || c == '\v' || c == '\f';
}

inline bool is_digit_char(u8 c)
{
    return c >= '0' && c <= '9';
}

inline bool is_name_char(u8 c)
{
    return (c >= 'a' && c <= 'z') || (c == '_') || (c >= 'A' && c <= 'Z') || is_digit_char(c);
}

inline void Lexer::eat_whitespace()
{
    while (is_whitespace(peek_char())) {
        eat_char();
    }
}

inline void Lexer::eat_name()
{
    while (peek_char() == '_' || is_name_char(peek_char())) {
        eat_char();
    }
}

inline s64 Lexer::eat_int_value()
{
    s64 value = 0;
    while (is_digit_char(peek_char())) {
        s64 digit = peek_char() - '0';
        eat_char();
        if (value > (INT64_MAX - digit) / 10) {
            printf("integer is too big\n");
            while (is_digit_char(peek_char())) {
                eat_char();
            }
            return 0;
        }
        value = value * 10 + digit;
    }
    return value;
}

Token Lexer::eat_token()
{
    Token token;
    token.l0 = current_line;
    token.c0 = current_column;

lex:
    // clang-format off
    switch (peek_char()) {
    case ' ': case '\n': case '\r': case '\t': case '\v': case '\f': {
        eat_whitespace();
        token.l0 = current_line;
        token.c0 = current_column;
        goto lex;
    }
    case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i':
    case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r':
    case 's': case 't': case 'u': case 'v': case 'w': case 'x': case 'y': case 'z': case 'A':
    case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J':
    case 'K': case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S':
    case 'T': case 'U': case 'V': case 'W': case 'X': case 'Y': case 'Z': case '_': {
        u8* start = current;
        eat_name();
        u8* end = current;
        token.kind = TOK_NAME;
        token.interned_name = intern_string(start, end);
        break;
    }
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
        token.kind = TOK_INT;
        token.int_value = eat_int_value();
        break;
    }
    default: {
        token.kind = (Token_Kind)peek_char();
        eat_char();
        break;
    }
    }
    // clang-format on

    token.l1 = current_line;
    token.c1 = current_column;
    return token;
}

void Lexer::lex()
{
    Time_Measurer measurer;
    measurer.start();
    while (true) {
        Token token = eat_token();
        if (token.kind == TOK_EOF) {
            break;
        }
        output.emplace_back(token);
    }
    measurer.stop();
    printf("Lexed %d lines in %.2fms (%.2fms / 1kloc)\n", processed_lines, measurer.elapsed(),
        measurer.elapsed() * 1000.0 / processed_lines);
    // print_info();
}

void Lexer::print_info()
{
    for (const Token& t : output) {
        switch (t.kind) {
        case TOK_NAME: {
            std::string name = intern_vector.at(t.interned_name);
            printf("TOK_NAME [%d:%d - %d:%d] = %llu: '%s'\n", t.l0, t.c0, t.l1, t.c1, t.interned_name, name.c_str());
            break;
        }
        default: {
            if (t.kind >= 32 && t.kind <= 126) {
                printf("TOK_ASCII = '%c'\n", t.kind);
            } else {
                printf("TOK_ASCII = %d\n", t.kind);
            }
            break;
        }
        }
    }
}
