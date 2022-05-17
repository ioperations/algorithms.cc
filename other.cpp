#include <cmath>
#include <iostream>
#include <sstream>

#include "array.h"
#include "box_drawing_chars.h"
#include "random.h"
#include "rich_text.h"
#include "string_utils.h"

using Entry = Rich_text::Entry<int>;
using Style = Rich_text::Style;

template <typename It>
void print_sequence(const It& b, const It& e) {
    for (auto el = b; el != e; ++el) std::cout << *el << " ";
}

template <typename It>
void sequential_search(It b, It e, int value) {
    std::cout << "searching for " << value << std::endl;
    int index = -1;
    int i = 0;
    for (auto el = b; index == -1 && el != e; ++el, ++i) {
        el->set_style(Style::bold());
        if (el->value == value) index = i;
    }
    print_sequence(b, e);
    std::cout << std::endl << "index = " << index << std::endl;
}

void quick_search(Array<Entry>& array, int value) {
    std::cout << "searching for " << value << std::endl;
    size_t i = 0;
    size_t j = array.size() - 1;
    size_t index = -1;
    while (index == static_cast<size_t>(-1) && i < j) {
        auto middle = i + (j - i) / 2;
        array[middle].set_style(Style::bold());
        print_sequence(array.cbegin(), array.cend());
        array[middle].remove_style(Style::bold());
        std::cout << std::endl;
        if (value == array[middle].value)
            index = middle;
        else if (value < array[middle].value)
            j = middle;
        else
            i = middle + 1;
    }
    std::cout << "index: " << static_cast<int>(index) << std::endl;
}

void eratosthenes_sieve(int n) {
    Array<bool> a1(n);
    for (int i = 2; i < n; ++i) a1[i] = true;
    for (int i = 2; i < n; ++i)
        if (a1[i])
            for (int j = i; j * i < n; j++) a1[i * j] = 0;
    for (int i = 2; i < n; ++i)
        if (a1[i]) std::cout << i << " ";
}

void coin_flipping_simulation() {  // coin-flipping simulation
    int histogram_point_count = 20;
    int flip_count = 500;
    Array<int> heads(histogram_point_count + 1);
    for (auto& i : heads) i = 0;
    int head_count;
    for (int i = 0; i < flip_count; ++i, ++heads[head_count]) {
        head_count = 0;
        for (int j = 0; j <= histogram_point_count; ++j)
            if (rand() < RAND_MAX / 2) ++head_count;
    }
    for (int i = 0; i <= histogram_point_count; ++i) {
        if (heads[i] == 0)
            std::cout << ".";
        else
            for (int j = 0; j < heads[i]; j += 10) std::cout << "*";
        std::cout << std::endl;
    }
}

struct Point {
    float m_x, m_y;
    Point() : Point(0, 0) {}
    Point(float x, float y) : m_x(x), m_y(y) {}
    float distance(const Point& o) const {
        float dx = m_x - o.m_x, dy = m_y - o.m_y;
        return sqrt(dx * dx + dy * dy);
    }
};

void count_points_within_distance(const Array<Point>& points,
                                  float max_distance) {
    int count = 0;
    for (size_t i = 0; i < points.size(); ++i)
        for (size_t j = i + 1; j < points.size(); ++j) {
            float distance = points[i].distance(points[j]);
            if (distance < max_distance) ++count;
        }
    std::cout << count << " pairs within " << max_distance << std::endl;
}

template <typename T>
struct Node {
    Node* m_next;
    T m_data;
    Node(Node* next, T data) : m_next(next), m_data(data) {}
    void print() {
        for (Node* node = this; node; node = node->m_next)
            std::cout << node->m_data << " ";
        std::cout << std::endl;
    }
};

template <typename T>
void delete_forward_list(Node<T>* head) {
    for (auto node = head; node;) {
        auto previous = node;
        node = node->m_next;
        delete previous;
    }
}

void count_points_within_distance_using_grids(const Array<Point>& points,
                                              float max_distance) {
    int g = 1 / max_distance;
    Array<Array<Node<Point>*>> grid(g + 2);
    for (auto& item : grid) {
        item = Array<Node<Point>*>(g + 2);
        for (auto& point : item) point = nullptr;
    }
    int count = 0;
    for (auto* point = points.cbegin(); point != points.cend(); ++point) {
        size_t grid_x = point->m_x * g + 1;
        size_t grid_y = point->m_y * g + 1;
        for (size_t i = grid_x - 1; i <= grid_x + 1; ++i)
            for (size_t j = grid_y - 1; j <= grid_y + 1; ++j)
                for (Node<Point>* node = grid[i][j]; node; node = node->m_next)
                    if (node->m_data.distance(*point) < max_distance) ++count;
        grid[grid_x][grid_y] = new Node<Point>(grid[grid_x][grid_y], *point);
    }
    std::cout << count << " pairs within " << max_distance << std::endl;
    for (auto& item : grid)
        for (auto& point : item) delete_forward_list(point);
}

struct CirtularList {
    struct Node {
        Node* m_next;
        int m_data;
    };
    Node* m_head = nullptr;
    Node* m_tail = nullptr;
    void add(int i) {
        Node* node = new Node;
        node->m_data = i;
        if (m_tail)
            m_tail->m_next = node;
        else
            m_head = node;
        m_tail = node;
        node->m_next = m_head;
    }
    void print() {
        namespace bc = Box_drawing_chars;
        if (m_head) {
            std::stringstream ss;
            ss << bc::right_bottom << m_head->m_data;
            for (Node* node = m_head->m_next; node && node != m_head;
                 node = node->m_next) {
                ss << bc::h_line << node->m_data;
            }
            ss << bc::left_bottom;
            std::cout << ss.str();
            std::cout << std::endl << bc::right_top;
            for (size_t i = 0; i < string_actual_printed_length(ss.str()) - 2;
                 ++i)
                std::cout << bc::h_line;
            std::cout << bc::left_top;
        }
    }
    ~CirtularList() {
        if (m_head) {
            for (Node* node = m_head->m_next; node && node != m_head;) {
                Node* previous = node;
                node = node->m_next;
                delete previous;
            }
            delete m_head;
        }
    }
};

void josephus_problem() {
    std::cout << "Josephus problem" << std::endl;
    Node<int>* head = new Node<int>(nullptr, 10);
    auto* node = head;
    for (int i = 11; i < 30; ++i)
        node = (node->m_next = new Node<int>(nullptr, i));
    node->m_next = head;

    auto print_circle = [](Node<int>* head, std::ostream& stream) {
        stream << head->m_data;
        for (auto* node = head->m_next; node != head; node = node->m_next)
            stream << " " << node->m_data;
    };

    node = head;
    while (node != node->m_next) {
        for (int i = 0; i < 5; ++i) node = node->m_next;
        auto* to_remove = node->m_next;
        node->m_next = node->m_next->m_next;
        std::cout << "[" << to_remove->m_data << "] ";
        std::stringstream ss;
        print_circle(node->m_next, ss);
        std::cout << ss.str() << std::endl;
        delete to_remove;
    }
    std::cout << "[" << node->m_data << "]" << std::endl;

    delete node;
}

void list_reversal() {
    std::cout << "reversing list" << std::endl;
    Node<int>* head = new Node<int>(nullptr, 10);
    auto* node = head;
    for (int i = 11; i < 21; ++i)
        node = (node->m_next = new Node<int>(nullptr, i));
    head->print();
    {
        Node<int>* previous = nullptr;
        for (auto* current = head; current;) {
            auto* next = current->m_next;
            current->m_next = previous;
            previous = current;
            current = next;
        }
        head = previous;
    }
    head->print();
    delete_forward_list(head);
}

template <typename It>
void list_insertion_sort(It begin, It end) {
    auto it = begin;
    if (it == end) return;
    ++it;
    if (it == end) return;

    it = begin;
    Node<int>* head = new Node<int>(nullptr, *begin);
    for (++it; it != end; ++it) {
        if (*it < head->m_data)
            head = new Node<int>(head, *it);
        else {
            Node<int>* node;
            for (node = head; node->m_next; node = node->m_next)
                if (*it < node->m_next->m_data) break;
            node->m_next = new Node<int>(node->m_next, *it);
        }
    }
    head->print();
    delete_forward_list(head);
}

int recursive_max(const Array<int> a, size_t l, size_t r) {
    if (l == r) return a[l];
    auto middle = (l + r) / 2;
    auto max_l = recursive_max(a, l, middle);
    auto max_r = recursive_max(a, middle + 1, r);
    return max_l > max_r ? max_l : max_r;
}

int main() {
    RandomSequenceGenerator generator(300, 10, 99);

    Array<Entry> array(15);
    Forward_list<Entry> list;
    for (auto& e : array) {
        int value = generator.generate();
        e.value = value;
        list.emplace_back(value);
    }

    Array<int> ints(array.size());
    for (size_t i = 0; i < array.size(); ++i) ints[i] = array[i].value;

    std::cout << "array sequential search" << std::endl;
    Rich_text::remove_styles(array.begin(), array.end());
    sequential_search(array.cbegin(), array.cend(), 58);

    std::cout << "list sequential search" << std::endl;
    Rich_text::remove_styles(list.begin(), list.end());
    sequential_search(list.begin(), list.end(), 58);

    std::cout << "array quick search" << std::endl;
    auto do_quick_search = [&array](int value) {
        Rich_text::remove_styles(array.begin(), array.end());
        quick_search(array, value);
    };
    do_quick_search(58);
    do_quick_search(12);
    do_quick_search(100);

    eratosthenes_sieve(100);

    coin_flipping_simulation();

    Array<Point> points(200);
    {
        auto random = []() { return ((float)rand()) / RAND_MAX; };
        for (auto& point : points) {
            point.m_x = random();
            point.m_y = random();
        }
    }
    count_points_within_distance(points, 0.015);
    count_points_within_distance_using_grids(points, 0.015);

    CirtularList c_list;
    c_list.add(1);
    c_list.add(2);
    c_list.add(3);
    c_list.print();
    std::cout << std::endl;

    josephus_problem();
    list_reversal();
    std::cout << "max = " << recursive_max(ints, 0, ints.size() - 1)
              << std::endl;

    std::cout << "list insertion_sort" << std::endl;
    auto ints_copy = ints;
    list_insertion_sort(ints_copy.begin(), ints_copy.end());
}
