#pragma once

#include <algorithm>
#include <iostream>

template<typename T>
class Forward_list {
    private:
        class Node;
        Node* head_;
        Node* tail_;

        void append_node(Node* node) {
            if (tail_) tail_->next_ = node;
            else head_ = node;
            tail_ = node;
        }
        void remove_nodes() {
            for (Node* node = head_; node; ) {
                Node* previous = node;
                node = node->next_;
                delete previous;
            }
        }
    public:
        class Iterator;
        class Const_iterator;
        using iterator = Iterator;
        using const_iterator = Const_iterator;

        Forward_list() :head_(nullptr), tail_(nullptr) {};

        Forward_list(const Forward_list&) = delete;
        Forward_list& operator=(const Forward_list&) = delete;

        Forward_list(Forward_list&& o) :head_(o.head_), tail_(o.tail_) {
            o.head_ = nullptr;
            o.tail_ = nullptr;
        }
        Forward_list& operator=(Forward_list&& o) {
            std::swap(head_, o.head_);
            std::swap(tail_, o.tail_);
            return *this;
        }

        ~Forward_list() { remove_nodes(); }

        template<typename... Args>
            void emplace_back(Args&&... args) {
                append_node(new Node(std::forward<Args>(args)...));
            }

        template<typename TT>
            void push_back(TT&& value) {
                append_node(new Node(std::forward<TT>(value)));
            }

        T& front() { return head_->value_; }
        T& back() { return tail_->value_; }
        iterator begin() {
            return Iterator(head_);
        }
        iterator end() {
            static Iterator it(nullptr);
            return it;
        }
        const_iterator cbegin() const {
            return Const_iterator(head_);
        }
        const_iterator cend() const {
            static Const_iterator it(nullptr);
            return it;
        }
        iterator before_end() {
            return Iterator(tail_);
        }
        bool empty() const {
            return head_ == nullptr;
        }
        void clear() {
            remove_nodes();
            head_ = nullptr;
            tail_ = nullptr;
        }
};

template<typename T>
struct Forward_list<T>::Node {
    T value_;
    Node* next_;
    template<typename TT>
        Node(TT&& value) :value_(std::forward<TT>(value)), next_(nullptr) {}
    template<typename... Args>
        Node(Args&&... args) :value_(std::forward<Args>(args)...), next_(nullptr) {}
};

template<typename N>
class Base_iterator {
    protected:
        N node_;
    public:
        Base_iterator(N node) :node_(node) {}
        bool empty() const { return node_ == nullptr; }
        Base_iterator& operator++() { 
            node_ = node_->next_; 
            return *this;
        }
        bool operator==(const Base_iterator& o) { return node_ == o.node_; }
        bool operator!=(const Base_iterator& o) { return node_ != o.node_; }
        bool operator==(Base_iterator&& o) { return node_ == o.node_; }
        bool operator!=(Base_iterator&& o) { return node_ != o.node_; }
};

template<typename T>
class Forward_list<T>::Iterator : public Base_iterator<Node*> {
    private:
        using Base = Base_iterator<Node*>;
    public:
        Iterator(Node* node) :Base(node) {}
        T& operator*() { return Base::node_->value_; }
        T* operator->() { return &Base::node_->value_; }
};

template<typename T>
class Forward_list<T>::Const_iterator : public Base_iterator<const Node*> {
    private:
        using Base = Base_iterator<const Node*>;
    public:
        Const_iterator(Node* node) :Base(node) {}
        const T& operator*() { return Base::node_->value_; }
        const T* operator->() { return &Base::node_->value_; }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, const Forward_list<T>& a) {
    stream << "[";
    auto it = a.cbegin();
    if (it != a.cend()) {
        stream << *it;
        for (++it; it != a.cend(); ++it)
            stream << ", " << *it;
    }
    return stream << "]";
}
