#include "common.h"
#include "lexer.h"

#include <cstdio>
#include <cstdlib>

static byte *read_whole_file(FILE *file) {
    fseek(file, 0L, SEEK_END);
    u64 file_size = ftell(file);
    rewind(file);

    byte *buf = (byte *)malloc(file_size + 1);
    if (!buf) return nullptr;

    u64 read_size = fread(buf, 1, file_size, file);
    if (read_size != file_size) return nullptr;

    buf[read_size] = 0;
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        logger("usage: %s file", argv[0]);
        return 2;
    }

    const char *filename = argv[1];

    FILE *file;
    auto err = fopen_s(&file, filename, "rb");
    if (err != 0) {
        logger("unable to open file: %s\n", filename);
        return 1;
    }

    byte *input = read_whole_file(file);
    fclose(file);
    if (!input) {
        logger("unable to read file: %s\n", filename);
        return 1;
    }

    Lexer lexer(input);
    lexer.lex();
    lexer.print_tokens();

    free(input);
    return 0;
}
