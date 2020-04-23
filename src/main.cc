#ifndef NDEBUG
#define PS_HH_LEAK_CHECK
#endif

#define PS_HH_IMPLEMENTATION
#include "ps.hh"

int main() {
    defer(mem_leak_check());
    return 0;
}
