#pragma once

#include <type_traits>

namespace Collections {

template <typename T, bool T_is_const>
class ReverseIterator {
   private:
    using value_type = std::conditional_t<T_is_const, const T, T>;
    value_type* m_ptr;

   public:
    ReverseIterator(value_type* ptr) : m_ptr(ptr) {}
    value_type& operator*() const { return *m_ptr; }
    value_type* operator->() const { return m_ptr; }
    bool operator==(const ReverseIterator& o) const { return m_ptr == o.m_ptr; }
    bool operator!=(const ReverseIterator& o) const { return !operator==(o); }
    ReverseIterator& operator++() {
        --m_ptr;
        return *this;
    }
};

}  // namespace Collections
