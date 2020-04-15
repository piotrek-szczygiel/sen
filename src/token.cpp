#include "token.h"

std::string Token::info() const {
    std::string result;

    switch (kind) {
    case TOK_ERR: result = "TOK_ERR"; break;
    case TOK_IDENT: result = "TOK_IDENT"; break;
    case TOK_NUMBER: result = "TOK_NUMBER"; break;
    case TOK_ARROW: result = "TOK_ARROW"; break;
    case TOK_EOF: result = "TOK_EOF"; break;
    default: result = "TOK_ASCII"; break;
    }

    result += " '" + std::string(start, end) + "'";
    return result;
}
