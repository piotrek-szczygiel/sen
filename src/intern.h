#pragma once

#include <unordered_map>
#include <vector>

#include "common.h"

using Interned = u64;

struct Intern_Table {
    Interned intern(const std::string& str);
    std::string unintern(Interned str);

    std::unordered_map<std::string, Interned> map;
    std::vector<std::string> vec;
};
