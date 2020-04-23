#include "ps.hh"

int main() {
    defer(mem_check_dump());
    return 0;
}
