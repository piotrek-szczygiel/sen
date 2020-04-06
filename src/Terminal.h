#pragma once

void term_init();

void term_error(const char* format, ...);
void term_info(const char* format, ...);
void term_reset();

enum class Color {
    FG_BLACK = 30,
    FG_RED = 31,
    FG_GREEN = 32,
    FG_YELLOW = 33,
    FG_BLUE = 34,
    FG_MAGENTA = 35,
    FG_CYAN = 36,
    FG_GRAY = 37,

    FG_BR_BLACK = 90,
    FG_BR_RED = 91,
    FG_BR_GREEN = 92,
    FG_BR_YELLOW = 93,
    FG_BR_BLUE = 94,
    FG_BR_MAGENTA = 95,
    FG_BR_CYAN = 96,
    FG_BR_GRAY = 97,

    FG_RESET = 39,

    BG_BLACK = 40,
    BG_RED = 41,
    BG_GREEN = 42,
    BG_YELLOW = 43,
    BG_BLUE = 44,
    BG_MAGENTA = 45,
    BG_CYAN = 46,
    BG_GRAY = 47,

    BG_BR_BLACK = 100,
    BG_BR_RED = 101,
    BG_BR_GREEN = 102,
    BG_BR_YELLOW = 103,
    BG_BR_BLUE = 104,
    BG_BR_MAGENTA = 105,
    BG_BR_CYAN = 106,
    BG_BR_GRAY = 107,

    BG_RESET = 49,
};

void term_color_stdout(Color color);
void term_color_stderr(Color color);
