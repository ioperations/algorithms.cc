#pragma once

template<typename T>
class List {
    private:
        class Node;
        Node* head_ = nullptr;
        Node* tail_ = nullptr;
    public:
        class Iterator;
        using iterator = Iterator;
        ~List() { delete head_; }
        void push_back(T&& value) {
            if (tail_) {
                tail_->next_ = new Node(value);
                tail_ = tail_->next_;
            } else {
                tail_ = new Node(value);
                head_ = tail_;
            }
        }
        iterator begin() {
            return Iterator(head_);
        }
        iterator end() {
            return Iterator(nullptr);
        }
};

template<typename T>
struct List<T>::Node {
    T value_;
    Node* next_;
    Node(T value) :value_(value), next_(nullptr) {}
    ~Node() { delete next_; }
};

template<typename T>
struct List<T>::Iterator {
    Node* node_;
    Iterator(Node* node) :node_(node) {}
    Iterator& operator++() { 
        node_ = node_->next_; 
        return *this;
    }
    bool operator!=(Iterator& o) { return node_ != o.node_; }
    T& operator*() { return node_->value_; }
};
