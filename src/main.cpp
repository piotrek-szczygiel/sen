#include "common.h"
#include "lexer.h"

#include <cstdio>
#include <cstdlib>

static byte *read_whole_file(const char *filename) {
    FILE *file;
    auto err = fopen_s(&file, filename, "rb");
    if (err != 0) return nullptr;
    defer { fclose(file); };

    fseek(file, 0L, SEEK_END);
    u64 file_size = ftell(file);
    rewind(file);

    u64 buf_size = file_size + 1;
    byte *buf = (byte *)malloc(buf_size);
    if (!buf) return nullptr;

    u64 read_size = fread_s(buf, buf_size, 1, file_size, file);
    if (read_size != file_size) return nullptr;

    buf[buf_size - 1] = 0;
    return buf;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        logger("usage: %s file", argv[0]);
        return 2;
    }

    const char *filename = argv[1];
    byte *input = read_whole_file(filename);
    if (!input) {
        logger("unable to open file: %s", filename);
        return 1;
    }

    defer { free(input); };

    Lexer lexer(input);
    lexer.lex();

    for (const auto &token : lexer.tokens) {
        // printf("%s\n", token.info().c_str());
    }

    return 0;
}
