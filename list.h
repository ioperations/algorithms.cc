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
            Node* node = new Node(std::move(value));
            if (tail_) {
                tail_->next_ = node;
                tail_ = tail_->next_;
            } else {
                tail_ = node;
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
    Node(T&& value) :value_(std::move(value)), next_(nullptr) {}
    ~Node() { delete next_; }
};

template<typename T>
class List<T>::Iterator {
    private:
        Node* node_;
    public:
        Iterator(Node* node) :node_(node) {}
        Iterator& operator++() { 
            node_ = node_->next_; 
            return *this;
        }
        bool operator!=(Iterator& o) { return node_ != o.node_; }
        T& operator*() { return node_->value_; }
};
