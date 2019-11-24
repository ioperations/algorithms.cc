#pragma once

#include <iostream>
#include <bitset>
#include <climits>
#include <array>

#include "std_ext.h"

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

    // todo move to Sequence class
    template<typename It>
        void remove_styles(It b, It e) {
            for (auto it = b; it != e; ++it)
                it->remove_styles();
        }

    template<typename T, std::size_t SZ>
        struct Styled_entries {
            const Style style_;
            const std::array<T*, SZ> entries_;
            Styled_entries(const Style& style, std::array<T*, SZ>&& entries) 
                :style_(style), entries_(entries) 
            {
                Std_ext::for_each(entries_, [this](auto e) {
                    e->add_style(style_); });
            }
            ~Styled_entries() {
                Std_ext::for_each(entries_, [this](auto e) {
                    e->remove_style(style_); });
            }
        };

    template<typename... Args>
        inline auto styled_entries(const Style& style, Args&... args) {
            return Styled_entries(style, Std_ext::make_pointers_array(args...));
        }

    template<typename It>
        class Sequence {
            private:
                It begin_;
                It end_;
            public:
                Sequence(const It& begin, const It& end) 
                    :begin_(begin), end_(end) 
                {}
                void reset(const It& begin, const It& end) {
                    begin_ = begin;
                    end_ = end;
                }
                void print(std::ostream& stream) {
                    auto el = begin_;
                    stream << *el;
                    for (++el; el != end_; ++el) {
                        stream << " " << *el;
                    }
                }
                template<typename... ES>
                    void print_with_styled_entry(std::ostream& stream, const Style& style, ES&... entries) {
                        auto se = styled_entries(style, entries...);
                        print(stream);
                    }
                template<typename... ES>
                    void print_with_styled_entry(const Style& style, ES&... entries) {
                        print_with_styled_entry(std::cout, style, entries...);
                    }
                template<typename... SES>
                    void print_with_styled_entries(std::ostream& stream, SES&&... styled_entries) {
                        print(stream);
                    }
                template<typename... SES>
                    void print_with_styled_entries(SES&&... styled_entries) {
                        print_with_styled_entries(std::cout, std::forward<SES>(styled_entries)...);
                    }
                const It& begin() { return begin_; }
                const It& end() { return end_; }
        };
}
