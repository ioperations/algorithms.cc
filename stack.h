#include <iostream>

template <typename T>
class Stack {
   private:
    struct Node {
        Node* m_next;
        T m_data;
        template <typename TT>
        Node(Node* next, TT&& data)
            : m_next(next), m_data(std::forward<TT>(data)) {}
    };
    Node* m_head;
    template <typename TT>
    friend std::ostream& operator<<(std::ostream& stream, Stack<TT>& stack);

   public:
    Stack() : m_head(nullptr) {}
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;
    ~Stack() {
        for (Node* node = m_head; node;) {
            Node* previous = node;
            node = node->m_next;
            delete previous;
        }
    }
    template <typename TT>
    void push(TT&& data) {
        m_head = new Node(m_head, std::forward<TT>(data));
    }
    template <typename... Args>
    void emplace(Args&&... args) {
        m_head = new Node(m_head, T(std::forward<Args>(args)...));
    }
    T pop() {
        Node* node = m_head;
        m_head = m_head->m_next;
        T data = std::move(node->m_data);
        node->m_next = nullptr;
        delete node;
        return data;
    }
    bool empty() { return m_head == nullptr; }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, Stack<T>& stack) {
    stream << "[";
    for (auto node = stack.m_head; node != nullptr; node = node->next_) {
        if (node != stack.m_head) stream << ", ";
        stream << node->data_;
    }
    stream << "]";
    return stream;
}
