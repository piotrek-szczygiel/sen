#include "Terminal.h"
#include <cstdarg>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#include <windows.h>

void enable_windows_colors(HANDLE h)
{
    if (h == INVALID_HANDLE_VALUE) {
        return;
    }
    DWORD dwMode = 0;
    if (!GetConsoleMode(h, &dwMode)) {
        return;
    }
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(h, dwMode)) {
        return;
    }
}
#endif

void term_init()
{
#ifdef _WIN32
    HANDLE hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE hStderr = GetStdHandle(STD_ERROR_HANDLE);
    
    enable_windows_colors(hStdout);
    enable_windows_colors(hStderr);
#endif
    
    std::atexit(term_reset);
}

void term_color_stdout(Color color)
{
    printf("\033[%dm", (int)color);
}

void term_color_stderr(Color color)
{
    fprintf(stderr, "\033[%dm", (int)color);
}

void term_error(const char* format, ...)
{
    term_color_stderr(Color::FG_BR_RED);
    va_list args;
    va_start(args, format);
    fprintf(stderr, "error: ");
    vfprintf(stderr, format, args);
    va_end(args);
    term_color_stderr(Color::FG_RESET);
}

void term_info(const char* format, ...)
{
    term_color_stderr(Color::FG_BR_CYAN);
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    term_color_stderr(Color::FG_RESET);
}

void term_reset()
{
    term_color_stdout(Color::FG_RESET);
    term_color_stdout(Color::BG_RESET);
    term_color_stderr(Color::FG_RESET);
    term_color_stderr(Color::BG_RESET);
}
