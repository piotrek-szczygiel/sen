#include "../src/Lexer.hh"
#include "../src/Token.hh"

int main(int argc, char** argv)
{
    Lexer l(R"~(
        test := 15;
    )~");

    if (!l.run())
        return 1;

    std::vector<u8> expected { TOK_NAME, ':', '=', TOK_INT, ';', TOK_EOF };
    if (l.output.size() != expected.size())
        return 1;

    for (u64 i = 0; i < l.output.size(); ++i)
        if (l.output[i].kind != expected[i])
            return 1;

    return 0;
}
