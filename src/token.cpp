#include "token.h"

std::string Token::info() const {
    std::string result;

    switch (kind) {
    case TOK_ERR: result = "TOK_ERR"; break;
    case TOK_IDENT: result = "TOK_IDENT"; break;
    case TOK_ARROW: result = "TOK_ARROW"; break;
    case TOK_DEF_ASSIGN: result = "TOK_DEF_ASSIGN"; break;
    case TOK_EOF: result = "TOK_EOF"; break;
    case TOK_I64: {
        result = "TOK_I64(" + std::to_string(v_i64) + ")";
    } break;
    case TOK_STR: {
        result = "TOK_STR(" + std::to_string(v_str) + ")";
    } break;
    default: {
        if (kind < 256) {
            result = "TOK_ASCII";
        } else {
            result = "TOK_UNKNOWN";
        }
    } break;
    }

    result += " '" + std::string(start, end) + "'";
    return result;
}
