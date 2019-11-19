#pragma once

#include <iostream>
#include <bitset>
#include <climits>

namespace Rich_text {

    class Style {
        private:
            using code_type = unsigned char;
            code_type code_;

            Style(code_type code);

            template<char code>
                static const Style create();
        public:
            static const Style normal();
            static const Style bold();
            static const Style red_bg();

            code_type code() const { return code_; }

            Style operator+(const Style& o) const;
            Style& operator+=(const Style& o);

            Style operator-=(const Style& o);

            void apply_to(std::ostream& stream);

            friend std::ostream& operator<<(std::ostream& stream, const Style& style);
    };

    template<typename T>
        class Entry {
            public:
                T value_;
            private:
                Style style_;
            public:
                Entry() :Entry(T()) {}
                Entry(const T& value) :Entry(value, Style::normal())  {}
                Entry(const T& value, const Style& style) :value_(value), style_(style) {}
                bool operator<(const Entry& o) const {
                    return value_ < o.value_;
                }
                void set_style(const Style& style) { style_ = style; }
                void add_style(const Style& style) { style_ += style; }
                void remove_style(const Style& style) { style_ -= style; }
                void remove_styles() { style_ = Style::normal(); }

                template<typename TT>
                    friend std::ostream& operator<<(std::ostream& s, Entry<TT> r) {
#ifdef unix
                        r.style_.apply_to(s);
#endif
                        s << r.value_;
#ifdef unix
                        if (r.style_.code())
                            s << "\x1B[0m";
#endif
                        return s;
                    }
        };

    template<typename It>
        void remove_styles(It b, It e) {
            for (auto it = b; it != e; ++it)
                it->remove_styles();
        }

}
