#pragma once

#include "list.h"

template<typename T>
class Array {
    private:
        T* ptr_;
        size_t size_;

        struct Builder;
        template<typename B, typename TT, typename... Args>
            static void do_build_array(B&& builder, TT&& t, Args&&... args);
        template<typename B>
            static void do_build_array(B&& builder);
    public:
        using iterator = T*;

        Array() :Array(nullptr, 0) {}
        Array(T* ptr, size_t size) :ptr_(ptr), size_(size) {}

        Array(const Array&) = delete;
        Array& operator=(const Array&) = delete;

        Array(Array&& o) :ptr_(o.ptr_), size_(o.size_) {
            o.ptr_ = nullptr;
            o.size_ = 0;
        }
        Array& operator=(Array&& o) {
            std::swap(ptr_, o.ptr_);
            std::swap(size_, o.size_);
            return *this;
        }

        ~Array() { delete[] ptr_; }

        iterator begin() { return ptr_; }
        iterator end() { return ptr_ + size_; }

        template<typename... Args>
            static auto build_array(Args&&... args);
};

template<typename T>
template<typename... Args>
auto Array<T>::build_array(Args&&... args) {
    using builder_type = typename Array<T>::Builder;
    auto builder = builder_type();
    do_build_array(std::forward<builder_type>(builder), std::forward<Args>(args)...);
    return builder.build();
}

template<typename T>
template<typename B, typename TT, typename... Args>
void Array<T>::do_build_array(B&& builder, TT&& t, Args&&... args) {
    builder.add(std::move(t));
    do_build_array(std::forward<B>(builder), std::forward<Args>(args)...);
}

template<typename T>
template<typename B>
void Array<T>::do_build_array(B&& builder) {
}

template<typename T>
struct Array<T>::Builder {
    List<T> list_;
    size_t count_ = 0;
    void add(T&& value) {
        list_.push_back(std::move(value));
        ++count_;
    }
    Array build() {
        Array a(new T[count_], count_);
        int i = -1;
        for (auto& v : list_) a.ptr_[++i] = std::move(v);
        return a;
    }
};

