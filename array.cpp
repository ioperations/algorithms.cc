#include "array.h"

#include <cstddef>

#include "math.h"

template <>
Array<bool> ArrayBuilder<Array<bool>, bool>::build() {
    Array<bool> a(m_count);
    int index = -1;
    for (bool b : m_list) {  // todo optimize?
        a[++index] = b;
    }
    return a;
}

Array<bool>::Array(size_t size)
    : m_actual_size(divide_round_up_int(size, static_cast<size_t>(CHAR_BIT))),
      m_ptr(new unsigned char[m_actual_size]),
      m_size(size) {}

Array<bool>::Array(size_t size, bool value) : Array(size) {
    if (value)
        for (auto* p = m_ptr; p != m_ptr + m_actual_size; *p = UCHAR_MAX, ++p)
            ;
    else
        for (auto* p = m_ptr; p != m_ptr + m_actual_size; *p = 0, ++p)
            ;
}
