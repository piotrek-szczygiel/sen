#pragma once
#include <cstdint>

#ifdef _WIN32
#include <windows.h>
#endif

using u8 = uint8_t;
using u64 = uint64_t;
using s64 = int64_t;
using f32 = float;
using f64 = double;

struct Time_Measurer {
    void start()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&t1);
#endif
    }

    void stop()
    {
#ifdef _WIN32
        QueryPerformanceCounter(&t2);
#endif
    }
    double elapsed()
    {
#ifdef _WIN32
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        return (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
#else
        return 0.0;
#endif
    }

#ifdef _WIN32
    LARGE_INTEGER t1;
    LARGE_INTEGER t2;
#endif
};
