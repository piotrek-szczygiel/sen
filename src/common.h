#pragma once

#include <cstdint>
#include <cstdio>

// Logging function
#define logger(...) fprintf(stderr, __VA_ARGS__)

// Some type aliases
using i64 = int64_t;
using u64 = uint64_t;
using i32 = int32_t;
using u32 = uint32_t;

using byte = uint8_t;

using f64 = double;
using f32 = float;

using Interned_String = u64;
