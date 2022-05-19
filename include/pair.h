#pragma once

#include <iostream>

template <typename F, typename S>
struct Pair {
    F m_first;
    S m_second;
    Pair() = default;
    Pair(F first, S second) : m_first(first), m_second(second) {}
    bool operator==(const Pair& o) const {
        return m_first == o.m_first && m_second == o.m_second;
    }
};

template <typename F, typename S>
std::ostream& operator<<(std::ostream& stream, const Pair<F, S>& pair) {
    return stream << "{" << pair.m_first << ": " << pair.m_second << "}";
}
