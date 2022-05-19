#pragma once

#include <climits>

#include "collections.h"
#include "forward_list.h"

template <typename A, typename T>
class ArrayBuilder {
   private:
    ForwardList<T> m_list;
    size_t m_count = 0;

   public:
    template <typename... Args>
    void emplace(Args&&... args) {
        m_list.emplace_back(std::forward<Args>(args)...);
        ++m_count;
    }
    template <typename TT>
    void add(TT&& t) {
        m_list.push_back(std::forward<TT>(t));
        ++m_count;
    }
    template <typename TT, typename... Args>
    void add(TT&& t, Args&&... value) {
        m_list.push_back(std::forward<TT>(t));
        ++m_count;
        add(std::forward<Args>(value)...);
    }
    A build() {
        A a(new T[m_count], m_count);
        int i = -1;
        for (auto& v : m_list) a.m_ptr[++i] = std::move(v);
        return a;
    }
    template <typename... Args>
    static A build_array(Args&&... args) {
        ArrayBuilder<A, T> builder;
        builder.add(std::forward<Args>(args)...);
        return builder.build();
    }
};

template <typename T>
class Array {
   private:
    T* m_ptr;
    size_t m_size;
    Array(T* ptr, size_t size) : m_ptr(ptr), m_size(size) {}

   public:
    using Builder = ArrayBuilder<Array, T>;
    friend class ArrayBuilder<Array, T>;

    using iterator = T*;
    using const_iterator = T* const;
    using reverse_iterator = Collections::ReverseIterator<T, false>;
    using const_reverse_iterator = Collections::ReverseIterator<T, true>;

    Array() : Array(nullptr, 0) {}
    explicit Array(size_t size) : Array(new T[size], size) {}

    Array(size_t size, const T& t) : Array(size) { fill(t); }

    Array(const std::initializer_list<T>& i_list) : Array(i_list.size()) {
        size_t i = -1;
        for (auto& el : i_list) m_ptr[++i] = el;
    }

    Array(const Array& o) : Array(o.m_size) {
        for (size_t i = 0; i < m_size; ++i) m_ptr[i] = o.m_ptr[i];
    }
    Array& operator=(const Array& o) {
        delete[] m_ptr;
        m_ptr = new T[o.m_size];
        m_size = o.m_size;
        for (size_t i = 0; i < m_size; ++i) m_ptr[i] = o.m_ptr[i];
        return *this;
    }

    Array(Array&& o) : m_ptr(o.m_ptr), m_size(o.m_size) {
        o.m_ptr = nullptr;
        o.m_size = 0;
    }
    Array& operator=(Array&& o) {
        std::swap(m_ptr, o.m_ptr);
        std::swap(m_size, o.m_size);
        return *this;
    }

    ~Array() { delete[] m_ptr; }

    T& operator[](size_t index) { return m_ptr[index]; }
    const T& operator[](size_t index) const { return m_ptr[index]; }

    template <typename... Args>
    void emplace(int index, Args&&... args) {
        m_ptr[index] = T(std::forward<Args>(args)...);
    }

    size_t size() const { return m_size; }

    void fill(const T& t) {
        for (auto& e : *this) e = t;
    }

    iterator begin() { return m_ptr; }
    iterator end() { return m_ptr + m_size; }
    const_iterator cbegin() const { return m_ptr; }
    const_iterator cend() const { return m_ptr + m_size; }
    reverse_iterator rbegin() { return {m_ptr + m_size - 1}; }
    reverse_iterator rend() { return {m_ptr - 1}; }
    const_reverse_iterator crbegin() const { return {m_ptr + m_size - 1}; }
    const_reverse_iterator crend() const { return {m_ptr - 1}; }

    template <typename... Args>
    static auto build_array(Args&&... args) {
        return ArrayBuilder<Array<T>, T>::build_array(
            std::forward<Args>(args)...);
    }
};

template <typename T>
bool operator==(const Array<T>& a1, const Array<T>& a2) {
    if (a1.size() != a2.size()) return false;
    for (size_t i = 0; i < a1.size(); ++i)
        if (a1[i] != a2[i]) return false;
    return true;
}

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Array<T>& a) {
    stream << "[";
    auto it = a.cbegin();
    if (it != a.cend()) {
        stream << *it;
        for (++it; it != a.cend(); ++it) stream << ", " << *it;
    }
    return stream << "]";
}

template <>
class Array<bool> {
   public:
    using Builder = ArrayBuilder<Array, bool>;
    friend class ArrayBuilder<Array, bool>;
    class Reference {
       private:
        friend class Array;
        bool m_value;
        unsigned char* m_char_ptr;
        int m_bit_index;
        void set_index(unsigned char* ptr, size_t index) {
            m_char_ptr = ptr + index / CHAR_BIT;
            m_bit_index = index % CHAR_BIT;
            m_value = *m_char_ptr & (0x80 >> m_bit_index);
        }
        Reference() : m_value(false), m_char_ptr(nullptr), m_bit_index(0){};

       public:
        Reference(bool value)
            : m_value(value), m_char_ptr(nullptr), m_bit_index(0) {}
        Reference(const Reference&) = delete;
        Reference& operator=(const Reference& o) = delete;
        Reference(Reference&&) = delete;
        Reference& operator=(Reference&& o) {
            if (o.m_value)
                *m_char_ptr |= (0x80 >> m_bit_index);
            else
                *m_char_ptr &= ~(0x80 >> m_bit_index);
            return *this;
        }
        operator bool() const { return m_value; }
    };

   private:
    template <typename R>
    class BaseIterator {
       private:
        friend class Array;
        unsigned char* const m_ptr;
        size_t m_index;
        Reference m_reference;
        BaseIterator(unsigned char* ptr, size_t index)
            : m_ptr(ptr), m_index(index) {}

       public:
        void operator++() { ++m_index; };
        bool operator==(const BaseIterator& o) { return m_index == o.m_index; }
        bool operator!=(const BaseIterator& o) { return m_index != o.m_index; }
        R& operator*() {
            m_reference.set_index(m_ptr, m_index);
            return m_reference;
        }
    };
    size_t m_actual_size;
    unsigned char* m_ptr;
    size_t m_size;
    Reference m_current_reference;

   public:
    using iterator = BaseIterator<Reference>;
    using const_iterator = BaseIterator<const Reference>;
    Array() : m_actual_size(0), m_ptr(nullptr), m_size(0) {}
    Array(size_t size);
    Array(size_t size, bool value);
    Array(const Array& o)
        : m_actual_size(o.m_actual_size),
          m_ptr(new unsigned char[m_actual_size]),
          m_size(o.m_size) {
        auto* p = m_ptr;
        auto* p_o = o.m_ptr;
        for (; p != m_ptr + m_actual_size; *p = *p_o, ++p, ++p_o)
            ;
    }
    Array& operator=(const Array& o) {
        auto copy = o;
        std::swap(*this, copy);
        return *this;
    }
    Array(Array&& o)
        : m_actual_size(o.m_actual_size), m_ptr(o.m_ptr), m_size(o.m_size) {
        o.m_ptr = nullptr;
    }
    Array& operator=(Array&& o) {
        std::swap(m_actual_size, o.m_actual_size);
        std::swap(m_ptr, o.m_ptr);
        std::swap(m_size, o.m_size);
        return *this;
    }
    ~Array() { delete[] m_ptr; }

    Reference& operator[](size_t index) {
        m_current_reference.set_index(m_ptr, index);
        return m_current_reference;
    };

    void fill(bool b) {
        for (auto& e : *this) e = b;
    }

    iterator begin() { return iterator(m_ptr, 0); };
    iterator end() { return iterator(m_ptr, m_size); };
    const_iterator cbegin() const { return const_iterator(m_ptr, 0); };
    const_iterator cend() const { return const_iterator(m_ptr, m_size); };
    template <typename... Args>
    static Array<bool> build_array(Args&&... args) {
        return ArrayBuilder<Array<bool>, bool>::build_array(
            std::forward<Args>(args)...);
    }
};

template <>
Array<bool> ArrayBuilder<Array<bool>, bool>::build();
