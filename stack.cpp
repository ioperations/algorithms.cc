#include <iostream>

#include "print_utils.h"

using namespace Print_utils;

template<typename T>
class Stack {
    private:
        struct Node {
            T data_;
            Node* next_;
            template<typename TT>
                Node(TT& data, Node* next) :data_(data), next_(next) {}
            ~Node() { delete next_; }
        };
        Node* head_;
        template<typename TT>
            friend std::ostream& operator<<(std::ostream& stream, Stack<TT>& stack);
    public:
        Stack() :head_(nullptr) {}
        Stack(const Stack&) = delete;
        Stack& operator=(const Stack&) = delete;
        ~Stack() { delete head_; }
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

int main() {
    {
        Stack<int> stack;
        printf("empty: {}\n", stack.empty());
        stack.push(1);
        stack.push(2);

        printf("stack: {}\n", stack);

        printf("empty: {}\n", stack.empty());
        std::cout << stack.pop() << std::endl;
        std::cout << stack.pop() << std::endl;
        printf("empty: {}\n", stack.empty());
    }

    {
        const char* infix = "5 * ( ( ( 9 + 8 ) * ( 4 * 6 ) ) + 7 )";
        std::cout << infix << std::endl;
        Stack<char> stack;
        const char* p;
        char c;
        for (p = infix; c != '\0'; ++p) {
            c = *p;
            if (c == ')')
                std::cout << stack.pop();
            else if (c == '+' || c == '*')
                stack.push(c);
            else if (c >= '0' && c <= '9') {
                std::cout << c;
            }
        }
        while (!stack.empty())
            std::cout << stack.pop();
    }
}
