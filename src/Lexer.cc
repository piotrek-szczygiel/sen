#include "Lexer.hh"
#include "Token.hh"

#include <cctype> // isalnum

bool Lexer::run()
{
    while (*cur) {
        switch (*cur) {
        case ' ':
        case '\t':
        case '\n': {
            ++cur;
            continue;
        }
        case 'a':
        case 'b':
        case 'c':
        case 'd':
        case 'e':
        case 'f':
        case 'g':
        case 'h':
        case 'i':
        case 'j':
        case 'k':
        case 'l':
        case 'm':
        case 'n':
        case 'o':
        case 'p':
        case 'q':
        case 'r':
        case 's':
        case 't':
        case 'u':
        case 'v':
        case 'w':
        case 'x':
        case 'y':
        case 'z':
        case 'A':
        case 'B':
        case 'C':
        case 'D':
        case 'E':
        case 'F':
        case 'G':
        case 'H':
        case 'I':
        case 'J':
        case 'K':
        case 'L':
        case 'M':
        case 'N':
        case 'O':
        case 'P':
        case 'Q':
        case 'R':
        case 'S':
        case 'T':
        case 'U':
        case 'V':
        case 'W':
        case 'X':
        case 'Y':
        case 'Z':
        case '_': {
            Token t(TOK_NAME);
            t.start = cur;
            while (isalnum(*cur)) {
                ++cur;
            }
            t.end = cur;
            output.push_back(t);
            break;
        }
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
            Token t(TOK_INT);
            t.start = cur;
            while (isdigit(*cur)) {
                ++cur;
            }
            t.end = cur;
            output.push_back(t);
            break;
        }
        default: {
            Token t(*cur);
            t.start = cur++;
            t.end = cur;
            output.push_back(t);
            break;
        }
        }
    }

    output.push_back(Token(TOK_EOF));

    for (auto& token : output) {
        printf("%s\n", token.to_str().c_str());
    }

    return true;
}
