#pragma once

#include <sstream>
#include <string>

size_t string_actual_printed_length(const std::string& str);

class StringBuilder {
   private:
    std::stringstream ss;

   public:
    template <typename T>
    StringBuilder& operator+(T&& t) {
        ss << std::forward<T>(t);
        return *this;
    }
    operator std::string() { return ss.str(); }
};
StringBuilder operator""_str(const char* s, long unsigned int);
