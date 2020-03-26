#pragma once

template<typename T>
std::string stringify(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

