#pragma once

#include <iostream>

#include "collections.h"

template <typename T>
class Vector {
   private:
    static const constexpr size_t default_size = 100;
    static const constexpr int size_multiplier = 2;

    T* m_array;
    size_t m_array_size;
    size_t m_size;

    void fill_defaults() {
        for (size_t i = 0; i < m_array_size; ++i) m_array[i] = T();
    }

   public:
    using iterator = T*;
    using const_iterator = const iterator;
    using reverse_iterator = Collections::ReverseIterator<T, false>;
    using const_reverse_iterator = Collections::ReverseIterator<T, true>;

    explicit Vector(size_t size)
        : m_array(new T[size]), m_array_size(size), m_size(size) {
        fill_defaults();
    }
    Vector()
        : m_array(new T[default_size]), m_array_size(default_size), m_size(0) {
        fill_defaults();
    }
    Vector(std::initializer_list<T> i_list) : Vector(i_list.size()) {
        size_t i = 0;
        for (auto& el : i_list) m_array[i++] = std::move(el);
    }

    Vector(const Vector& o)
        : m_array(new T[o.m_array_size]),
          m_array_size(o.m_array_size),
          m_size(o.m_size) {
        for (size_t i = 0; i < m_size; ++i) m_array[i] = o.m_array[i];
    }
    Vector& operator=(const Vector& o) {
        m_array_size = o.m_array_size;
        m_size = o.m_size;
        delete[] m_array;
        m_array = new T[m_array_size];
        for (size_t i = 0; i < m_size; ++i) m_array[i] = o.m_array[i];
        return *this;
    }

    Vector(Vector&& o)
        : m_array(o.m_array), m_array_size(o.m_array_size), m_size(o.m_size) {
        o.m_array = nullptr;
        o.m_array_size = 0;
        o.m_size = 0;
    }
    Vector& operator=(Vector&& o) {
        std::swap(m_array, o.m_array);
        std::swap(m_array_size, o.m_array_size);
        std::swap(m_size, o.m_size);
        return *this;
    };

    ~Vector() { delete[] m_array; }

    inline size_t size() const { return m_size; }

    inline iterator begin() { return m_array; }
    inline iterator end() { return m_array + m_size; }
    inline const_iterator cbegin() const { return m_array; }
    inline const_iterator cend() const { return m_array + m_size; }

    reverse_iterator rbegin() { return {m_array + m_size - 1}; }
    reverse_iterator rend() { return {m_array - 1}; }
    const_reverse_iterator crbegin() const { return {m_array + m_size - 1}; }
    const_reverse_iterator crend() const { return {m_array - 1}; }

    template <typename TT>
    void push_back(TT&& t) {
        if (m_size + 1 > m_array_size) {
            m_array_size *= size_multiplier;
            T* old_array = m_array;
            m_array = new T[m_array_size];
            for (size_t i = 0; i < m_size; ++i)
                m_array[i] = std::move(old_array[i]);
            delete[] old_array;
        }
        m_array[m_size++] = std::forward<TT>(t);
    }
    template <typename... Args>
    void emplace_back(Args&&... args) {
        push_back(T(std::forward<Args>(args)...));
    }

    T& operator[](size_t i) { return m_array[i]; }

    const T& operator[](size_t i) const { return m_array[i]; }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const Vector<T>& vector) {
    auto el = vector.cbegin();
    stream << "[";
    if (el != vector.cend()) {
        stream << *el;
        for (++el; el != vector.cend(); ++el) stream << ", " << *el;
    }
    stream << "]";
    return stream;
}
