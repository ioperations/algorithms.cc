#include "rich_text.h"

namespace Rich_text {

Style::Style(code_type code) : m_code(code) {}

template <char code>
const Style Style::create() {
    static Style s(code);
    return s;
}

const Style Style::normal() { return create<0b00>(); }
const Style Style::bold() { return create<0b01>(); }
const Style Style::red_bg() { return create<0b10>(); }

Style Style::operator+(const Style& o) const {
    return Style(m_code | o.m_code);
}
Style& Style::operator+=(const Style& o) {
    m_code |= o.m_code;
    return *this;
}

Style Style::operator-=(const Style& o) {
    m_code &= ~o.m_code;
    return *this;
}

void Style::apply_to(std::ostream& stream) {
    Style::code_type mask = 1;
    if (mask & m_code) stream << "\x1B[1m";
    mask <<= 1;
    if (mask & m_code) stream << "\x1B[41m";
}

std::ostream& operator<<(std::ostream& stream, const Style& style) {
    stream << std::bitset<CHAR_BIT>(style.m_code);
    return stream;
}

}  // namespace Rich_text
