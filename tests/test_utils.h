#pragma once

template<typename T>
std::string stringify(const T& t) {
    std::stringstream ss;
    ss << t;
    return ss.str();
}

std::stringstream& reset_with_new_line(std::stringstream& ss) {
    ss.str("");
    ss << std::endl;
    return ss;
}

std::stringstream& reset(std::stringstream& ss) {
    ss.str("");
    return ss;
}
