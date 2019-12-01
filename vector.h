#pragma once

#include <iostream>

template<typename T>
class Vector {
    private:
        static const constexpr size_t DEFAULT_SIZE = 100;
        static const constexpr int SIZE_MULTIPLIER = 2;

        T* array_;
        size_t array_size_;
        size_t size_;

        Vector(size_t array_size, size_t size) 
            :array_(new T[array_size]), array_size_(array_size), size_(size) 
        {}

        void grow(size_t size) {
            array_size_ = size;
            T* old_array = array_;
            array_ = new T[array_size_];
            for (size_t i = 0; i < size_; ++i)
                array_[i] = std::move(old_array[i]);
            delete[] old_array;
        }
    public:
        Vector(size_t size) :Vector(size, 0) {}
        Vector() : Vector(DEFAULT_SIZE) {}
        Vector(std::initializer_list<T> i_list) :Vector(i_list.size()) {
            for (auto& el : i_list)
                array_[size_++] = std::move(el);
        }
        Vector(const Vector& o) :Vector(o.array_size_, o.size_) {
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

        inline size_t size() { return size_; }

        inline T* begin() { return array_; }
        inline T* end() { return array_ + size_; }
        inline const T* cbegin() const { return array_; }
        inline const T* cend() const { return array_ + size_; }

        template<typename TT>
        void push_back(TT&& t) {
            if (size_ + 1 > array_size_)
                grow(array_size_ * SIZE_MULTIPLIER);
            array_[size_++] = std::forward<TT>(t);
        }

        T& operator[](size_t i) {
            if (i >= array_size_)
                grow(i + DEFAULT_SIZE);
            for (size_t j = size_; j < i + 1; ++j) 
                array_[j] = T();
            size_ = std::max(i + 1, size_);
            return array_[i];
        }

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
