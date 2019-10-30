#include <memory>
#include <iostream>

template<typename T>
class Stack {
    private:
        struct Node {
            T data_;
            std::unique_ptr<Node> next_;
            Node(T& data, std::unique_ptr<Node>&& next)
                :data_(data), next_(std::move(next))
            {}
        };
        std::unique_ptr<Node> head_;
    public:
        void push(T&& t) {
            std::unique_ptr<Node> node(new Node(t, std::move(head_)));
            head_ = std::move(node);
        }
        T pop() {
            std::unique_ptr<Node> node = std::move(head_);
            head_ = std::move(node->next_);
            return node->data_;

        }
        bool empty() {
            return head_.get() == nullptr;
        }
};

template<typename T>
class Stack_2 {
    private:
        struct Node {
            T data_;
            Node* next_;
            Node(T& data, Node* next) :data_(data), next_(next) {}
            ~Node() { delete next_; }
        };
        Node* head_ = nullptr;
    public:
        ~Stack_2() { delete head_; }
        void push(T&& data) {
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

int main() {
    Stack_2<int> stack;
    std::cout << "empty: " << stack.empty() << std::endl;
    stack.push(1);
    stack.push(2);

    std::cout << "empty: " << stack.empty() << std::endl;
    // std::cout << stack.pop() << std::endl;
    // std::cout << stack.pop() << std::endl;
    // std::cout << "empty: " << stack.empty() << std::endl;
}
