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
    if (read_size != file_size) {
        free(buf);
        return nullptr;
    }

    buf[read_size] = 0;
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "usage: sen file");
        return 2;
    }

    const char *filename = argv[1];

    FILE *file;
    auto err = fopen_s(&file, filename, "rb");
    if (err != 0) {
        fprintf(stderr, "unable to open file: %s\n", filename);
        return 1;
    }
    defer { fclose(file); };

    byte *input = read_whole_file(file);
    if (!input) {
        fprintf(stderr, "unable to read file: %s\n", filename);
        return 1;
    }
    defer { free(input); };

    Lexer lexer(input, filename);
    lexer.lex();
    lexer.print_tokens();
    lexer.print_errors();

    return 0;
}
