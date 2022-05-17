#pragma once

#include <array>
#include <bitset>
#include <climits>
#include <iostream>

#include "std_ext.h"

namespace Rich_text {

class Style {
   private:
    using code_type = unsigned char;
    code_type m_code;

    Style(code_type code);

    template <char code>
    static const Style create();

   public:
    static const Style normal();
    static const Style bold();
    static const Style red_bg();

    code_type code() const { return m_code; }

    Style operator+(const Style& o) const;
    Style& operator+=(const Style& o);

    Style operator-=(const Style& o);

    void apply_to(std::ostream& stream);

    friend std::ostream& operator<<(std::ostream& stream, const Style& style);
};

template <typename T>
class Entry {
   public:
    T value;

   private:
    Style m_style;

   public:
    Entry() : Entry(T()) {}
    Entry(const T& value) : Entry(value, Style::normal()) {}
    Entry(const T& value, const Style& style) : value(value), m_style(style) {}
    bool operator<(const Entry& o) const { return value < o.value; }
    void set_style(const Style& style) { m_style = style; }
    void add_style(const Style& style) { m_style += style; }
    void remove_style(const Style& style) { m_style -= style; }
    void remove_styles() { m_style = Style::normal(); }

    template <typename TT>
    friend std::ostream& operator<<(std::ostream& s, Entry<TT> r) {
#ifdef unix
        r.m_style.apply_to(s);
#endif
        s << r.value;
#ifdef unix
        if (r.m_style.code()) s << "\x1B[0m";
#endif
        return s;
    }
};

template <typename It>
void remove_styles(It b, It e) {
    for (auto it = b; it != e; ++it) it->remove_styles();
}

template <typename T, std::size_t SZ>
struct StyledEntries {
    const Style m_style;
    const std::array<T*, SZ> m_entries;
    StyledEntries(const Style& style, std::array<T*, SZ>&& entries)
        : m_style(style), m_entries(entries) {
        Std_ext::for_each(m_entries, [this](auto e) { e->add_style(m_style); });
    }
    ~StyledEntries() {
        Std_ext::for_each(m_entries,
                          [this](auto e) { e->remove_style(m_style); });
    }
};

template <typename... Args>
inline auto styled_entries(const Style& style, Args&... args) {
    return StyledEntries(style, Std_ext::make_pointers_array(args...));
}

template <typename It>
class Sequence {
   private:
    It m_begin;
    It m_end;

   public:
    Sequence(const It& begin, const It& end) : m_begin(begin), m_end(end) {}
    void reset(const It& begin, const It& end) {
        m_begin = begin;
        m_end = end;
    }
    void print(std::ostream& stream) {
        auto el = m_begin;
        stream << *el;
        for (++el; el != m_end; ++el) {
            stream << " " << *el;
        }
    }
    template <typename... ES>
    void print_with_styled_entry(std::ostream& stream, const Style& style,
                                 ES&... entries) {
        auto se = styled_entries(style, entries...);
        print(stream);
    }
    template <typename... SES>
    void print_with_styled_entries(std::ostream& stream,
                                   SES&&... styled_entries) {
        print(stream);
    }
    const It& begin() { return m_begin; }
    const It& end() { return m_end; }
};
}  // namespace Rich_text
