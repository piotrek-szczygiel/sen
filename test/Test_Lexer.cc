#include "../src/Lexer.hh"
#include "../src/Terminal.hh"
#include <cstdio>

int main(int argc, char** argv)
{
    term_init();

    const char* filename = "../../test/Test_Lexer.sen";
    FILE* file = fopen(filename, "rb");
    if (file == nullptr) {
        term_error("unable to open file: %s\n", filename);
        return 1;
    } else {
        Lexer lexer;
        lexer.init();
        lexer.set_input_from_file(file);
        lexer.lex();
        lexer.print_info();
        lexer.free();
        fclose(file);
    }

    return 0;
}
