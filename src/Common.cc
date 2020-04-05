#include "Common.hh"

void Time_Measurer::start()
{
#ifdef _WIN32
    QueryPerformanceCounter(&t1);
#endif
}

void Time_Measurer::stop()
{
#ifdef _WIN32
    QueryPerformanceCounter(&t2);
#endif
}
double Time_Measurer::elapsed()
{
#ifdef _WIN32
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return (t2.QuadPart - t1.QuadPart) * 1000.0 / frequency.QuadPart;
#else
    return 0.0;
#endif
}
