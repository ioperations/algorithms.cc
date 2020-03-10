#pragma once

template<typename T>
class Array_queue {
    private:
        T* array_;
        const size_t size_;
        size_t back_;
        size_t front_;
        bool empty_;
        inline void update_position(size_t& p) { if (++p == size_) p = 0; }
        inline void update_back() {
            update_position(back_);
            empty_ = false;
        }
    public:
        Array_queue(size_t size) :array_(new T[size]), size_(size), back_(0), front_(0), empty_(true) {}
        ~Array_queue() { delete[] array_; }
        template<typename TT>
            void push(TT&& t) {
                array_[back_] = std::forward<TT>(t);
                update_back();
            }
        template<typename... Args>
            void emplace(Args&&... args) {
                array_[back_] = T(std::forward<Args>(args)...);
                update_back();
            }
        T pop() {
            T t = array_[front_];
            update_position(front_);
            if (front_ == back_) empty_ = true;
            return t;
        }
        bool empty() const { return empty_; }
};
