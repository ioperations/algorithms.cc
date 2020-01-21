#pragma once

template<typename T>
class Two_dimensional_array {
    public:
        template<bool T_is_const>
            class Row;
    private:
        size_t rows_count_;
        size_t columns_count_;
        size_t size_;
        T* begin_;
        T* end_;

        Two_dimensional_array(T* const begin, size_t rows, size_t columns, size_t size)
            :rows_count_(rows), columns_count_(columns), size_(size),
            begin_(begin), end_(begin_ + size) {}
        Two_dimensional_array(size_t rows, size_t columns, size_t size)
            :Two_dimensional_array(new T[size], rows, columns, size) {}

        template<typename It>
            It do_get_row(size_t index) const {
                auto p = begin_ + index * columns_count_;
                return {p, p + columns_count_};
            }
    public:
        using row_type = Row<false>;
        using const_row_type = Row<true>;
        using iterator = typename row_type::Iterator;
        using const_iterator = typename const_row_type::Iterator;

        Two_dimensional_array(size_t rows, size_t columns)
            :Two_dimensional_array(rows, columns, rows * columns)
        {}
        Two_dimensional_array(const Two_dimensional_array& o)
            :Two_dimensional_array(o.rows_count_, o.columns_count_, o.size_)
        {
            for (T* p = begin_, * o_p = o.begin_; p != end_; *p++ = *o_p++);
        }
        Two_dimensional_array& operator=(const Two_dimensional_array& o) {
            auto copy = o;
            std::swap(*this, copy);
            return *this;
        }
        Two_dimensional_array(Two_dimensional_array&& o)
            :Two_dimensional_array(o.begin_, o.rows_count_, o.columns_count_, o.size_)
        {
            o.begin_ = nullptr;
        }
        Two_dimensional_array& operator=(Two_dimensional_array&& o) {
            std::swap(begin_, o.begin_);
            std::swap(end_, o.end_);
            std::swap(rows_count_, o.rows_count_);
            std::swap(columns_count_, o.columns_count_);
            std::swap(size_, o.size_);
            return *this;
        }
        ~Two_dimensional_array() { delete[] begin_; }

        const T& get(size_t row, size_t column) const {
            return *(begin_ + row * columns_count_ + column);
        }
        T& get(size_t row, size_t column) {
            return const_cast<T&>(static_cast<const Two_dimensional_array&>(*this).get(row, column));
        }
        Row<false> operator[](size_t index) {
            return do_get_row<Row<false>>(index);
        }
        Row<true> operator[](size_t index) const {
            return do_get_row<Row<true>>(index);
        }
        void fill(const T& value) {
            for (auto p = begin_; p != end_; *p++ = value);
        }

        const_iterator cbegin() const { return {*this, begin_}; }
        const_iterator cend() const { return {*this, end_}; }
        iterator begin() { return {*this, begin_}; }
        iterator end() { return {*this, end_}; }
};

template<typename T>
template<bool T_is_const>
class Two_dimensional_array<T>::Row {
    public:
        class Iterator;
        friend class Iterator;
        friend class Two_dimensional_array;
    private:
        using value_type = std::conditional_t<T_is_const, const T, T>;
        value_type* begin_;
        value_type* end_;
        Row(value_type* begin, value_type* end) :begin_(begin), end_(end) {}
    public:
        value_type* begin() { return begin_; }
        value_type* end() { return end_; }
        value_type* cbegin() const { return begin_; }
        value_type* cend() const { return end_; }
        value_type& operator[](size_t index) {
            return *(begin_ + index);
        }
};

template<typename T>
template<bool T_is_const>
class Two_dimensional_array<T>::Row<T_is_const>::Iterator {
    private:
        friend class Two_dimensional_array<T>;
        const Two_dimensional_array<T>& array_;
        Row row_;
        Iterator(const Two_dimensional_array& array, T* ptr)
            :array_(array), row_(ptr, ptr + array_.columns_count_)
        {}
    public:
        Row& operator*() { return row_; }
        Row* operator->() { return &operator*(); }
        Iterator& operator++() {
            auto p = row_.begin_ + array_.columns_count_;
            row_ = {p, p + array_.columns_count_};
            return *this;
        }
        bool operator!=(const Iterator& o) const {
            return row_.begin_ != o.row_.begin_;
        }
        Row::value_type& operator[](size_t index) {
            return row_[index];
        }
};

