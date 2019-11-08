#pragma once

#include <climits>

#include "math.h"
#include "forward_list.h"

template<typename T>
class Array {
    private:
        T* ptr_;
        size_t size_;

        template<typename B, typename TT, typename... Args>
            static void do_build_array(B&& builder, TT&& t, Args&&... args);
        template<typename B>
            static void do_build_array(B&& builder);

        Array(T* ptr, size_t size) :ptr_(ptr), size_(size) {}
    public:
        struct Builder;

        using iterator = T*;
        using const_iterator = T* const;

        Array() :Array(nullptr, 0) {}
        Array(size_t size) :Array(new T[size], size) {}

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

        T& operator[](size_t index) { return ptr_[index]; }
        const T& operator[](size_t index) const { return ptr_[index]; }

        template<typename... Args>
            void emplace(int index, Args&&... args) {
                ptr_[index] = T(std::forward<Args>(args)...);
            }

        size_t size() const { return size_; }

        iterator begin() { return ptr_; }
        iterator end() { return ptr_ + size_; }
        const_iterator cbegin() const { return ptr_; } // todo code duplication
        const_iterator cend() const { return ptr_ + size_; }

        template<typename... Args>
            static auto build_array(Args&&... args);
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Array<T>& a) {
    stream << "[";
    auto it = a.cbegin();
    if (it != a.cend()) {
        stream << *it;
        for (++it; it != a.cend(); ++it)
            stream << ", " << *it;
    }
    return stream << "]";
}

template<>
struct Array<bool> {
    public:
        class Reference {
            private:
                friend class Array;
                bool value_;
                char* char_ptr_;
                int bit_index_;
                void set_index(char* ptr, size_t index) {
                    char_ptr_ = ptr + index / CHAR_BIT;
                    bit_index_ = index % CHAR_BIT;
                    value_ = *char_ptr_ & (0x80 >> bit_index_);
                }
                Reference() :value_(false), char_ptr_(nullptr), bit_index_(0) {};
            public:
                Reference(bool value) :value_(value), char_ptr_(nullptr), bit_index_(0) {
                }
                Reference(const Reference&) = delete;
                Reference& operator=(const Reference& o) = delete;
                Reference(Reference&&) = delete;
                Reference& operator=(Reference&& o) {
                    if (o.value_)
                        *char_ptr_ |= (0x80 >> bit_index_);
                    else
                        *char_ptr_ &= ~(0x80 >> bit_index_); 
                    return *this;
                }
                operator bool() const {
                    return value_;
                }
        };
        template<typename R>
        class Base_iterator {
            private:
                friend class Array;
                char* const ptr_;
                size_t index_;
                Reference reference_;
                Base_iterator(char* ptr, size_t index) :ptr_(ptr), index_(index) {}
            public:
                void operator++() {
                    ++index_;
                };
                bool operator==(const Base_iterator& o) {
                    return index_ == o.index_;
                }
                bool operator!=(const Base_iterator& o) {
                    return index_ != o.index_;
                }
                R& operator*() {
                    reference_.set_index(ptr_, index_);
                    return reference_;
                }
        };
    private:
        char* ptr_;
        size_t size_;
        size_t actual_size_;
        Reference current_reference_;
        Array(char* ptr, size_t size) 
            :ptr_(ptr), size_(size), actual_size_(divide_round_up(size, static_cast<size_t>(CHAR_BIT))) 
        {}
    public:
        using iterator = Base_iterator<Reference>;
        using const_iterator = Base_iterator<const Reference>;
        Array(size_t size) :Array(new char[size], size) {}

        Reference& operator[](size_t index) {
            current_reference_.set_index(ptr_, index);
            return current_reference_;
        };
        iterator begin() {
            return iterator(ptr_, 0);
        };
        iterator end() {
            return iterator(ptr_, size_);
        };
        const_iterator cbegin() const {
            return const_iterator(ptr_, 0);
        };
        const_iterator cend() const {
            return const_iterator(ptr_, size_);
        };
};

template<typename T>
template<typename... Args>
auto Array<T>::build_array(Args&&... args) {
    Array<T>::Builder builder;
    do_build_array(builder, std::forward<Args>(args)...);
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
void Array<T>::do_build_array(B&& builder) {}

template<typename T>
class Array<T>::Builder {
    private:
        Forward_list<T> list_;
        size_t count_ = 0;
    public:
        template<typename... Args>
            void emplace(Args&&... args) {
                list_.emplace_back(std::forward<Args>(args)...);
                ++count_;
            }
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

