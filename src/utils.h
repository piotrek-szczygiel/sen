#pragma once

// Go-like defer macro
#define CONCAT_INTERNAL(x, y) x##y
#define CONCAT(x, y) CONCAT_INTERNAL(x, y)
#define defer const auto &CONCAT(defer__, __LINE__) = Defer_Help() + [&]()

template <typename T> struct Defer {
    T lambda;
    Defer(T lambda) : lambda(lambda) {}

    ~Defer() { lambda(); }
    Defer(const Defer &);

  private:
    Defer &operator=(const Defer &);
};

struct Defer_Help {
    template <typename T> Defer<T> operator+(T t) { return t; }
};
