#pragma once
#include <cstdint>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif

using u8 = uint8_t;
using u64 = uint64_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;

struct Time_Measurer {
    void start();
    void stop();

    double elapsed();

#ifdef _WIN32
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
#else
    // Linux
#endif
};
