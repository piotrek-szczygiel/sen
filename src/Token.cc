#include "Token.hh"

std::string Token::to_str()
{
    switch (kind) {
    case TOK_EOF:
        return "TOK_EOF";
    case TOK_NAME:
        return "TOK_NAME";
    case TOK_INT:
        return "TOK_INT";
    default:
        return "'" + std::string(1, kind) + "'";
    }
}
