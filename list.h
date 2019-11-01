#pragma once

#include <algorithm>

template<typename T>
class List {
    private:
        class Node;
        Node* head_;
        Node* tail_;
    public:
        class Iterator;
        using iterator = Iterator;

        List() :head_(nullptr), tail_(nullptr) {};

        List(const List&) = delete;
        List& operator=(const List&) = delete;

        List(List&& o) :head_(o.head_), tail_(o.tail_) {
            o.head_ = nullptr;
            o.tail_ = nullptr;
        }
        List& operator=(List&& o) {
            std::swap(head_, o.head_);
            std::swap(tail_, o.tail_);
            return *this;
        }

        ~List() { delete head_; }

        template<typename TT>
            void push_back(TT&& value) {
                Node* node = new Node(std::forward<TT>(value));
                if (tail_) {
                    tail_->next_ = node;
                    tail_ = tail_->next_;
                } else {
                    tail_ = node;
                    head_ = tail_;
                }
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
        iterator before_end() {
            return Iterator(tail_);
        }
        bool empty() {
            return head_ == nullptr;
        }
};

template<typename T>
struct List<T>::Node {
    T value_;
    Node* next_;
    template<typename TT>
        Node(TT&& value) :value_(std::forward<TT>(value)), next_(nullptr) {}
    ~Node() { delete next_; }
};

template<typename T>
class List<T>::Iterator {
    private:
        Node* node_;
    public:
        Iterator(Node* node) :node_(node) {}
        bool empty() { return node_ == nullptr; }
        Iterator& operator++() { 
            node_ = node_->next_; 
            return *this;
        }
        bool operator!=(Iterator& o) { return node_ != o.node_; }
        bool operator!=(Iterator&& o) { return node_ != o.node_; }
        T& operator*() { return node_->value_; }
        T* operator->() { return &node_->value_; }
};
