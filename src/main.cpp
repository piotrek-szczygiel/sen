#include <cstdio>

#include "utils.h"

int main(int argc, char **argv) {
    defer { printf("Hello deffered!\n"); };

    printf("Hello world!\n");
    return 0;
}
