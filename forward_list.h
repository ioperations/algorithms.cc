#pragma once

#include <algorithm>
#include <iostream>

template <typename T>
class ForwardList {
   private:
    struct Node;
    Node* m_head;
    Node* m_tail;

    void append_node(Node* node) {
        if (m_tail)
            m_tail->m_next = node;
        else
            m_head = node;
        m_tail = node;
    }
    void add_all(const ForwardList& o) {
        for (auto node = o.m_head; node; node = node->m_next)
            push_back(node->m_value);
    }
    void remove_nodes() {
        for (Node* node = m_head; node;) {
            Node* previous = node;
            node = node->m_next;
            delete previous;
        }
    }

   public:
    template <bool T_is_const>
    class Iterator;
    using iterator = Iterator<false>;
    using const_iterator = Iterator<true>;

    ForwardList() : m_head(nullptr), m_tail(nullptr){};

    ForwardList(const std::initializer_list<T>& i_list) : ForwardList() {
        for (auto& item : i_list) push_back(item);
    }

    ForwardList(const ForwardList& o) : ForwardList() { add_all(o); }
    ForwardList& operator=(const ForwardList& o) {
        clear();
        add_all(o);
        return *this;
    }

    ForwardList(ForwardList&& o) : m_head(o.m_head), m_tail(o.m_tail) {
        o.m_head = nullptr;
        o.m_tail = nullptr;
    }
    ForwardList& operator=(ForwardList&& o) {
        std::swap(m_head, o.m_head);
        std::swap(m_tail, o.m_tail);
        return *this;
    }

    ~ForwardList() { remove_nodes(); }

    void merge_sort();

    template <typename... Args>
    void emplace_back(Args&&... args) {
        append_node(new Node(std::forward<Args>(args)...));
    }

    template <typename TT>
    void push_back(TT&& value) {
        append_node(new Node(std::forward<TT>(value)));
    }

    T& front() { return m_head->m_value; }
    T& back() { return m_tail->m_value; }
    T pop_front() {
        auto node = m_head;
        m_head = m_head->m_next;
        if (!m_head) m_tail = nullptr;
        auto t = node->m_value;
        delete node;
        return t;
    }
    iterator begin() { return iterator(m_head); }
    iterator end() {
        static iterator it(nullptr);
        return it;
    }
    const_iterator cbegin() const { return const_iterator(m_head); }
    const_iterator cend() const {
        static const_iterator it(nullptr);
        return it;
    }
    iterator before_end() { return iterator(m_tail); }
    template <typename F>
    bool remove_first_if(F f) {
        if (empty()) return false;

        Node* previous;
        Node* current = m_head;
        bool found = false;
        for (; !(found = f(current->m_value)) && current != m_tail;
             previous = current, current = current->m_next)
            ;
        if (!found) return false;

        if (current == m_head) {
            m_head = current->m_next;
            if (!m_head) m_tail = nullptr;
        } else {
            previous->m_next = current->m_next;
            if (current == m_tail) m_tail = previous;
        }
        delete current;
        return true;
    }
    bool empty() const { return m_head == nullptr; }
    void clear() {
        remove_nodes();
        m_head = nullptr;
        m_tail = nullptr;
    }
};

template <typename T>
struct ForwardList<T>::Node {
    T m_value;
    Node* m_next;
    template <typename TT>
    Node(TT&& value) : m_value(std::forward<TT>(value)), m_next(nullptr) {}
    template <typename... Args>
    Node(Args&&... args)
        : m_value(std::forward<Args>(args)...), m_next(nullptr) {}
};

template <bool T_is_true, typename T_true_type, typename T_false_type>
struct Choose_type;

template <typename T_true_type, typename T_false_type>
struct Choose_type<true, T_true_type, T_false_type> {
    using type = T_true_type;
};

template <typename T_true_type, typename T_false_type>
struct Choose_type<false, T_true_type, T_false_type> {
    using type = T_false_type;
};

template <typename T>
template <bool T_is_const>
class ForwardList<T>::Iterator {
   private:
    using node_type = typename Choose_type<T_is_const, const Node, Node>::type;
    using value_type = typename Choose_type<T_is_const, const T, T>::type;
    node_type* m_node;

   public:
    Iterator(node_type* node) : m_node(node) {}
    bool empty() const { return m_node == nullptr; }
    Iterator& operator++() {
        m_node = m_node->m_next;
        return *this;
    }
    bool operator==(const Iterator& o) const { return m_node == o.m_node; }
    bool operator!=(const Iterator& o) const { return !operator==(o); }
    value_type& operator*() const { return m_node->m_value; }
    value_type* operator->() const { return &operator*(); }
};

template <typename T>
std::ostream& operator<<(std::ostream& stream, const ForwardList<T>& a) {
    stream << "[";
    auto it = a.cbegin();
    if (it != a.cend()) {
        stream << *it;
        for (++it; it != a.cend(); ++it) stream << ", " << *it;
    }
    return stream << "]";
}

template <typename T>
void ForwardList<T>::merge_sort() {
    if (!m_head || m_head == m_tail) return;
    struct List {
        Node* m_head;
        size_t m_length = 0;
        List(Node* head, size_t length) : m_head(head), m_length(length) {}
    };
    List l(m_head, 0);
    for (Node* node = l.m_head; node; node = node->m_next) ++l.m_length;

    static struct {
        List merge(const List& l1, const List& l2) {
            Node* n1 = l1.m_head;
            Node* n2 = l2.m_head;
            auto append_node = [&n1, &n2]() {
                Node* n;
                if (n2->m_value < n1->m_value) {
                    n = n2;
                    n2 = n2->m_next;
                } else {
                    n = n1;
                    n1 = n1->m_next;
                }
                return n;
            };
            List l(append_node(), l1.m_length + l2.m_length);
            Node* n = l.m_head;
            while (n1 && n2) n = (n->m_next = append_node());
            if (n1)
                n->m_next = n1;
            else
                n->m_next = n2;
            return l;
        }
        List merge_sort(List& l1) {
            if (l1.m_length > 1) {
                size_t length = 0;
                Node* node = l1.m_head;
                for (; length < l1.m_length / 2 - 1; ++length)
                    node = node->m_next;
                ++length;
                List l2(node->m_next, l1.m_length - length);
                node->m_next = nullptr;
                l1.m_length = length;
                l1 = merge(merge_sort(l1), merge_sort(l2));
            }
            return l1;
        }
    } helper;
    l = helper.merge_sort(l);
    m_head = l.m_head;
    for (m_tail = l.m_head; m_tail->m_next; m_tail = m_tail->m_next)
        ;
}
