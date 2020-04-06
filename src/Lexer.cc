#include "Lexer.hh"
#include "Token.hh"

void Lexer::init()
{
    input_size = 0;
    input = nullptr;
    current = nullptr;
    current_line = 1;
    current_column = 1;
    processed_lines = 1;
    output.clear();
    intern_map.clear();
    intern_vector.clear();
    interned_keywords.clear();

    // Just making sure that some interned string won't have 0-id by mistake
    intern_vector.emplace_back("!!! INVALID !!!");

    std::unordered_map<std::string, Token_Kind> keywords = {
        { "int", TOK_KEY_INT },
        { "float", TOK_KEY_FLOAT },
        { "string", TOK_KEY_STRING },
        { "bool", TOK_KEY_BOOL },
        { "true", TOK_KEY_TRUE },
        { "false", TOK_KEY_FALSE },
        { "fn", TOK_KEY_FN },
        { "if", TOK_KEY_IF },
        { "else", TOK_KEY_ELSE },
        { "return", TOK_KEY_RETURN },
    };

    for (const auto& k : keywords) {
        interned_keywords.emplace(intern_string(k.first), k.second);
    }
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

u64 Lexer::intern_string(const std::string& string)
{
    auto found = intern_map.find(string);
    if (found == intern_map.end()) {
        u64 intern = intern_vector.size();
        intern_vector.emplace_back(string);
        intern_map.emplace(string, intern);
        return intern;
    } else {
        return found->second;
    }
}

u64 Lexer::intern_string(u8* start, u8* end)
{
    std::string string((char*)start, (size_t)(end - start));
    return intern_string(string);
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
        while (is_whitespace(peek_char())) {
            eat_char();
        }
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
        while (peek_char() == '_' || is_name_char(peek_char())) {
            eat_char();
        }
        u8* end = current;
        u64 interned = intern_string(start, end);
        auto find = interned_keywords.find(interned);
        if(find == interned_keywords.end()) {
            token.kind = TOK_ID;
            token.id = interned;
        } else {
            token.kind = find->second;
        }
        break;
    }
    case '0': case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': {
        token.kind = TOK_INT;
        token.int_value = eat_int_value();
        break;
    }
    case '"': {
        eat_char();
        u8* start = current;
        while (peek_char() && peek_char() != '"') {
            eat_char();
        }
        if(peek_char() == TOK_EOF) {
            printf("unterminated string at the eof\n");
            token.kind = TOK_ERROR;
            break;
        }
        u8* end = current;
        eat_char();
        u64 value = intern_string(start, end);
        token.kind = TOK_STRING;
        token.string_value = value;
        break;
    }
    case '/': {
        if(peek_char(1) == '/') {
            eat_char(2);
            while(peek_char() && peek_char() != '\n') {
                eat_char();
            }
            goto lex;
        } else if(peek_char(1) == '*') {
            eat_char(2);
            while(peek_char() && peek_char(1)) {
                if(peek_char() == '*' && peek_char(1) == '/') {
                    eat_char(2);
                    goto lex;
                }
                eat_char();
            }
        }
        goto ascii_token;
    }
    case '-': {
        if(peek_char(1) == '>') {
            token.kind = TOK_ARROW;
            eat_char(2);
            break;
        }
        goto ascii_token;
    }
    default: {
    ascii_token:
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
    measurer.start();
    while (true) {
        Token token = eat_token();
        if (token.kind == TOK_EOF) {
            break;
        }
        output.emplace_back(token);
    }
    measurer.stop();
    elapsed = measurer.elapsed();
}

std::string Lexer::token_info(Token* token)
{
    switch (token->kind) {
    case TOK_EOF:
        return "EOF";
    case TOK_ID:
        return "ID<" + intern_vector.at(token->id) + ">";
    case TOK_INT:
        return "INT<" + std::to_string(token->int_value) + ">";
    case TOK_FLOAT:
        return "FLOAT<" + std::to_string(token->float_value) + ">";
    case TOK_STRING:
        return "STRING<" + intern_vector.at(token->string_value) + ">";
    case TOK_KEY_INT:
        return "int";
    case TOK_KEY_FLOAT:
        return "float";
    case TOK_KEY_STRING:
        return "string";
    case TOK_KEY_BOOL:
        return "bool";
    case TOK_KEY_TRUE:
        return "true";
    case TOK_KEY_FALSE:
        return "false";
    case TOK_KEY_FN:
        return "fn";
    case TOK_KEY_IF:
        return "if";
    case TOK_KEY_ELSE:
        return "else";
    case TOK_KEY_RETURN:
        return "return";
    case TOK_ARROW:
        return "->";
    case TOK_ERROR:
        return "!!! ERROR !!!";
    default: {
        if (token->kind >= 32 && token->kind <= 126) {
            return "'" + std::string(1, (char)token->kind) + "'";
        } else {
            return "ASCII: " + std::to_string((int)token->kind);
        }
    }
    }
}

void Lexer::print_info()
{
    for (Token& t : output) {
        printf("%s\n", token_info(&t).c_str());
    }
}
