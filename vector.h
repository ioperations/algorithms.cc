#pragma once

#include <iostream>

template<typename T>
class Vector {
    private:
        static const constexpr size_t DEFAULT_SIZE = 100;
        static const constexpr int SIZE_MULTIPLIER = 2;

        T* array_;
        size_t array_size_;
        size_t size_; // todo size or current max index?

        void fill_defaults() {
            for (size_t i = 0; i < array_size_; ++i)
                array_[i] = T();
        }

        template<bool T_is_const>
            class Reverse_iterator;
    public:
        using iterator = T*;
        using const_iterator = const iterator;
        using reverse_iterator = Reverse_iterator<false>;
        using const_reverse_iterator = Reverse_iterator<true>;

        Vector(size_t size) :array_(new T[size]), array_size_(size), size_(size) { 
            fill_defaults();
        }
        Vector() :array_(new T[DEFAULT_SIZE]), array_size_(DEFAULT_SIZE), size_(0) { 
            fill_defaults();
        }
        Vector(std::initializer_list<T> i_list) :Vector(i_list.size()) {
            size_t i = 0;
            for (auto& el : i_list)
                array_[i++] = std::move(el);
        }

        Vector(const Vector& o) :array_(new T[o.array_size_]), array_size_(o.array_size_), size_(o.size_) {
            for (size_t i = 0; i < size_; ++i)
                array_[i] = o.array_[i];
        }
        Vector& operator=(const Vector& o) {
            array_size_ = o.array_size_;
            size_ = o.size_;
            delete[] array_;
            array_ = new T[array_size_];
            for (size_t i = 0; i < size_; ++i)
                array_[i] = o.array_[i];
            return *this;
        }
        
        Vector(Vector&& o) 
            :array_(o.array_), array_size_(o.array_size_), size_(o.size_) 
        {
            o.array_ = nullptr;
            o.array_size_ = 0;
            o.size_ = 0;
        }
        Vector& operator=(Vector&& o) {
            std::swap(array_, o.array_);
            std::swap(array_size_, o.array_size_);
            std::swap(size_, o.size_);
            return *this;
        };

        ~Vector() { delete[] array_; }

        inline size_t size() const { return size_; }

        inline iterator begin() { return array_; }
        inline iterator end() { return array_ + size_; }
        inline const_iterator cbegin() const { return array_; }
        inline const_iterator cend() const { return array_ + size_; }

        reverse_iterator rbegin() { return {array_ + size_ - 1}; }
        reverse_iterator rend() { return {array_ - 1}; }
        const_reverse_iterator crbegin() const { return {array_ + size_ - 1}; }
        const_reverse_iterator crend() const { return {array_ - 1}; }

        template<typename TT>
        void push_back(TT&& t) {
            if (size_ + 1 > array_size_) {
                array_size_ *= SIZE_MULTIPLIER;
                T* old_array = array_;
                array_ = new T[array_size_];
                for (size_t i = 0; i < size_; ++i)
                    array_[i] = std::move(old_array[i]);
                delete[] old_array;
            }
            array_[size_++] = std::forward<TT>(t);
        }

        T& operator[](size_t i) {
            return array_[i];
        }

        const T& operator[](size_t i) const {
            return array_[i];
        }
};

template<typename T>
template<bool T_is_const>
class Vector<T>::Reverse_iterator {
    private:
        using value_type = std::conditional_t<T_is_const, const T, T>;
        value_type* ptr_;
    public:
        Reverse_iterator(value_type* ptr) :ptr_(ptr) {}
        value_type& operator*() const { return *ptr_; }
        value_type* operator->() const { return ptr_; }
        bool operator==(const Reverse_iterator& o) const { return ptr_ == o.ptr_; }
        bool operator!=(const Reverse_iterator& o) const { return !operator==(o); }
        Reverse_iterator& operator++() { --ptr_; return *this; }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Vector<T>& vector) {
    auto el = vector.cbegin();
    stream << "[";
    if (el != vector.cend()) {
        stream << *el;
        for (++el; el != vector.cend(); ++el)
            stream << ", " << *el;
    }
    stream << "]";
    return stream;
}
