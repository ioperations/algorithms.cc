#pragma once

#include <iostream>

template<typename T>
class Rich_text {
    public:
        T value_;
        bool bold_;
        Rich_text() :value_(), bold_(false) {}
        template<typename TT>
            Rich_text(TT&& value, bool bold): value_(std::forward<TT>(value)), bold_(bold) {}
        bool operator<(const Rich_text& o) const {
            return value_ < o.value_;
        }
        template<typename TT>
            friend std::ostream& operator<<(std::ostream& s, Rich_text<TT> r) {
#ifdef unix
                if (r.bold_)
                    s << "\x1B[1m";
#endif
                s << r.value_;
#ifdef unix
                if (r.bold_)
                    s << "\x1B[0m";
#endif
                return s;
            }
};

template<typename It>
static void clear_rich_text_format(It b, It e) {
    for (auto it = b; it != e; ++it)
        it->bold_ = false;
}
