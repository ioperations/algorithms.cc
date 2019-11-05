#include "string_utils.h"

size_t string_length(const std::string& str) {
    int len = 0;
    auto s = str.c_str();
    while (*s) len += (*s++ & 0xc0) != 0x80;
    return len;
}
