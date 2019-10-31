#pragma once

#include "list.h"

template<typename T>
class Array {
    private:
        T* ptr_;
        size_t size_;
    public:
        class Builder;

        ~Array() { delete[] ptr_; }

        T* begin() { return ptr_; }
        T* end() { return ptr_ + size_; }
};

template<typename T>
class Array<T>::Builder {
    private:
        List<T> list_;
        size_t count_ = 0;
    public:
        Builder& add(T&& value) {
            list_.push_back(std::move(value));
            ++count_;
            return *this;
        }
        Array build() {
            Array a;
            a.ptr_ = new T[count_];
            a.size_ = count_;
            int i = -1;
            for (auto& v : list_) a.ptr_[++i] = v;
            return a;
        }
};
