#include "Token.hh"

std::string Token::to_str() const
{
    switch (kind) {
    case TOK_EOF:
        return "TOK_EOF";
    case TOK_NAME:
        return "TOK_NAME (" + std::string((char*)start, end - start) + ")";
    case TOK_INT:
        return "TOK_INT (" + std::to_string(s64_val) + ")";
    default:
        return "'" + std::string(1, kind) + "'";
    }
}
