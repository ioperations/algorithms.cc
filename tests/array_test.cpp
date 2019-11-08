#include "gtest/gtest.h"

#include "array.h"

#include <climits>
#include <iostream>
#include <bitset>

#include <vector>

template<typename T>
T ceil_int(T dividend, T divisor) {
    T mod = 0;
    for (; dividend > divisor; dividend -= divisor, ++mod);
    if (divisor - dividend != divisor) 
        ++mod;
    return mod;
}

using bitset_c = std::bitset<CHAR_BIT>;

struct Bool_array {
    public:
        class Reference {
            private:
                friend class Bool_array;
                bool value_;
                char* char_ptr_;
                int bit_index_;
                void set_index(char* ptr, size_t index) {
                    char_ptr_ = ptr + index / CHAR_BIT;
                    bit_index_ = index % CHAR_BIT;
                    value_ = *char_ptr_ & (0x80 >> bit_index_);
                }
                void set(bool value) {
                    if (value)
                        *char_ptr_ |= (0x80 >> bit_index_);
                    else
                        *char_ptr_ &= ~(0x80 >> bit_index_); 
                }
                Reference() :value_(false), char_ptr_(nullptr), bit_index_(0) {};
            public:
                Reference(bool value) :value_(value), char_ptr_(nullptr), bit_index_(0) {
                }
                Reference(const Reference&) = delete;
                Reference& operator=(const Reference& o) = delete;
                Reference(Reference&&) = delete;
                Reference& operator=(Reference&& o) {
                    set(o.value_);
                    return *this;
                }
                operator bool() {
                    return value_;
                }
        };
    private:
        char* ptr_;
        size_t size_;
        size_t actual_size_;
        Reference current_reference_;
        Bool_array(char* ptr, size_t size) 
            :ptr_(ptr), size_(size), actual_size_(ceil_int(size, static_cast<size_t>(CHAR_BIT))) 
        {}
    public:
        Bool_array(size_t size) :Bool_array(new char[size], size) {}

        void print() {
            for (auto b = ptr_; b != ptr_ + actual_size_; ++b)
                std::cout << bitset_c(*b) << std::endl;
        }

        Reference& operator[](size_t index) {
            current_reference_.set_index(ptr_, index);
            return current_reference_;
        };
};

TEST(Array_test, test_0) {
    Bool_array a(20);
    a.print();
    bool b = true;
    a[0] = true;
    a[10] = b;
    a[13] = b;
    a[19] = true;
    a.print();

    std::cout << a[10] << std::endl;
    a[10] = false;
    std::cout << a[10] << std::endl;

    a.print();

}

