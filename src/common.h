#pragma once

#include <cstdint>
#include <cstdio>

// Some type aliases
using i64 = int64_t;
using u64 = uint64_t;
using i32 = int32_t;
using u32 = uint32_t;

using byte = uint8_t;

using f64 = double;
using f32 = float;

using Interned_String = u64;

// Go-like defer macro
#define CONCAT_INTERNAL(x, y) x##y
#define CONCAT(x, y) CONCAT_INTERNAL(x, y)
#define defer                                                                  \
    [[maybe_unused]] const auto &CONCAT(defer__, __LINE__) =                   \
        Defer_Help() + [&]()

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
