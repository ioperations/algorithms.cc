#pragma once

#include <iostream>

template<typename F, typename S>
struct Pair {
    F first_;
    S second_;
    Pair() = default;
    Pair(F first, S second) :first_(first), second_(second) {}
};

template<typename F, typename S>
std::ostream& operator<<(std::ostream& stream, const Pair<F, S>& pair) {
    return stream << "{" << pair.first_ << ": " << pair.second_ << "}";
}
