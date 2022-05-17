#include <iostream>

template <typename T>
class Stack {
   private:
    struct Node {
        Node* next_;
        T data_;
        template <typename TT>
        Node(Node* next, TT&& data)
            : next_(next), data_(std::forward<TT>(data)) {}
    };
    Node* head_;
    template <typename TT>
    friend std::ostream& operator<<(std::ostream& stream, Stack<TT>& stack);

   public:
    Stack() : head_(nullptr) {}
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    ~Stack() {
        for (Node* node = head_; node;) {
            Node* previous = node;
            node = node->next_;
            delete previous;
        }
    }
    template <typename TT>
    void push(TT&& data) {
        head_ = new Node(head_, std::forward<TT>(data));
    }
    template <typename... Args>
    void emplace(Args&&... args) {
        head_ = new Node(head_, T(std::forward<Args>(args)...));
    }
    T pop() {
        Node* node = head_;
        head_ = head_->next_;
        T data = std::move(node->data_);
        node->next_ = nullptr;
        delete node;
        return data;
    }
    bool empty() { return head_ == nullptr; }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, Stack<T>& stack) {
    stream << "[";
    for (auto node = stack.head_; node != nullptr; node = node->next_) {
        if (node != stack.head_) stream << ", ";
        stream << node->data_;
    }
    stream << "]";
    return stream;
}
