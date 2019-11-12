#include <iostream>

template<typename T>
class Stack {
    private:
        struct Node {
            T data_;
            Node* next_;
            template<typename TT>
                Node(TT& data, Node* next) :data_(data), next_(next) {}
        };
        Node* head_;
        template<typename TT>
            friend std::ostream& operator<<(std::ostream& stream, Stack<TT>& stack);
    public:
        Stack() :head_(nullptr) {}
        Stack(const Stack&) = delete;
        Stack& operator=(const Stack&) = delete;
        ~Stack() {
            for (Node* node = head_; node; ) {
                Node* previous = node;
                node = node->next_;
                delete previous;
            }
        }
        template<typename TT>
            void push(TT&& data) {
                head_ = new Node(data, head_);
            }
        T pop() {
            Node* node = head_;
            head_ = head_->next_;
            T data = std::move(node->data_);
            node->next_ = nullptr;
            delete node;
            return data;
        }
        bool empty() {
            return head_ == nullptr;
        }
};

template<typename T>
std::ostream& operator<<(std::ostream& stream, Stack<T>& stack) {
    stream << "[";
    for (auto node = stack.head_; node != nullptr; node = node->next_) {
        if (node != stack.head_)
            stream << ", ";
        stream << node->data_;
    }
    stream << "]";
    return stream;
}
