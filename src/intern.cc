#include "intern.h"

Interned Intern_Table::intern(const std::string& str) {
    auto found = map.find(str);
    if (found == map.end()) {
        Interned interned = vec.size();
        vec.push_back(str);
        map.emplace(str, interned);
        return interned;
    } else {
        return found->second;
    }
}

std::string Intern_Table::unintern(Interned str) {
    if (str >= vec.size()) return "";
    return vec[str];
}
