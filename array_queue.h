#pragma once

#include <cstddef>
#include <utility>
template <typename T>
class ArrayQueue {
   private:
    T* m_array;
    const size_t m_size;
    size_t m_back;
    size_t m_front;
    bool m_empty;
    inline void update_position(size_t& p) {
        if (++p == m_size) p = 0;
    }
    inline void update_back() {
        update_position(m_back);
        m_empty = false;
    }

   public:
    ArrayQueue(size_t size)
        : m_array(new T[size]),
          m_size(size),
          m_back(0),
          m_front(0),
          m_empty(true) {}
    ~ArrayQueue() { delete[] m_array; }
    template <typename TT>
    void push(TT&& t) {
        m_array[m_back] = std::forward<TT>(t);
        update_back();
    }
    template <typename... Args>
    void emplace(Args&&... args) {
        m_array[m_back] = T(std::forward<Args>(args)...);
        update_back();
    }
    T pop() {
        T t = m_array[m_front];
        update_position(m_front);
        if (m_front == m_back) m_empty = true;
        return t;
    }
    bool empty() const { return m_empty; }
};
