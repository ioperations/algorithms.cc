#include <random>
#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "rich_text.h"
#include "box_drawing_chars.h"
#include "string_utils.h"

class Random_sequence_generator {
    private:
        std::mt19937 engine;
        std::uniform_int_distribution<std::mt19937::result_type> distribution;
    public:
        Random_sequence_generator(unsigned long seed, int b, int e)
            :engine(seed), distribution(b, e)
        {}
        Array<int> generate_array(size_t size) {
            Array<int> array(size);
            for (auto& e : array)
                e = generate();
            return array;
        }
        int generate() {
            return distribution(engine);
        }
};

template<typename It>
void print_sequence(const It& b, const It& e) {
    for (auto el = b; el != e; ++el)
        std::cout << *el << " ";
}

template<typename It>
void buble_sort(It b, It e) {
    int iteration = 0;
    auto print_iteration = [&b, &e, &iteration]() {
        print_sequence(b, e);
        std::cout << "[" << iteration << "]" << std::endl;
        ++iteration;
    };
    print_sequence(b, e);
    std::cout << std::endl;

    auto ee = e;
    for (auto it = b; it != e; ++it) {
        auto previous = b;
        auto current = previous;
        ++current;
        if (current != ee)
            do {
                if (current->value_ < previous->value_)
                    std::swap(current->value_, previous->value_);
                previous = current;
                ++current;
            } while (current != ee);
        ee = previous;
        ee->bold_ = true;
        print_iteration();
    }
}

template<typename It>
void sequential_search(It b, It e, int value) {
    std::cout << "searching for " << value << std::endl;
    int index = -1;
    int i = 0;
    for (auto el = b; index == -1 && el != e; ++el, ++i) {
        el->bold_ = true;
        if (el->value_ == value)
            index = i;
    }
    print_sequence(b, e);
    std::cout << std::endl << "index = " << index << std::endl;
}

using Entry = Rich_text<int>;

void quick_search(Array<Entry>& array, int value) {
    std::cout << "searching for " << value << std::endl;
    size_t i = 0;
    size_t j = array.size() - 1;
    size_t index = -1;
    while (index == static_cast<size_t>(-1) && i < j) {
        auto middle = i + (j - i) / 2;
        array[middle].bold_ = true;
        print_sequence(array.cbegin(), array.cend());
        array[middle].bold_ = false;
        std::cout << std::endl;
        if (value == array[middle].value_)
            index = middle;
        else if (value < array[middle].value_)
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
            for (int j = i; j * i < n; j++)
                a1[i * j] = 0;
    for (int i = 2; i < n; ++i)
        if (a1[i])
            std::cout << i << " ";
}

void coin_flipping_simulation() { // coin-flipping simulation
    int histogram_point_count = 20;
    int flip_count = 500;
    Array<int> heads(histogram_point_count + 1);
    for (auto& i : heads) 
        i = 0;
    int head_count;
    for (int i = 0; i < flip_count; ++i, ++heads[head_count]) {
        head_count = 0;
        for (int j = 0; j <= histogram_point_count; ++j)
            if (rand() < RAND_MAX / 2) 
                ++head_count;
    }
    for (int i = 0; i <= histogram_point_count; ++i) {
        if (heads[i] == 0) 
            std::cout << ".";
        else
            for (int j = 0; j < heads[i]; j += 10)
                std::cout << "*";
        std::cout << std::endl;
    }
}

struct Point {
    float x_, y_;
    Point() :Point(0, 0) {}
    Point(float x, float y) :x_(x), y_(y) {}
    float distance(const Point& o) const {
        float dx = x_ - o.x_, dy = y_ - o.y_;
        return sqrt(dx * dx + dy * dy);
    }
};

void count_points_within_distance(const Array<Point>& points, float max_distance) {
    int count = 0;
    for (size_t i = 0; i < points.size(); ++i)
        for (size_t j = i + 1; j < points.size(); ++j) {
            float distance = points[i].distance(points[j]);
            if (distance < max_distance)
                ++count;
        }
    std::cout << count << " pairs within " << max_distance << std::endl;
}

struct Cirtular_list {
    struct Node {
        Node* next_;
        int data_;
    };
    Node* head_ = nullptr;
    Node* tail_ = nullptr;
    void add(int i) {
        Node* node = new Node;
        node->data_ = i;
        if (tail_) tail_->next_ = node;
        else head_ = node;
        tail_ = node;
        node->next_ = head_;
    }
    void print() {
        namespace bc = Box_drawing_chars;
        if (head_) {
            std::stringstream ss;
            ss << bc::right_bottom << head_->data_;
            for (Node* node = head_->next_; node && node != head_;
                 node = node->next_) {
                ss << bc::h_line << node->data_;
            }
            ss << bc::left_bottom;
            std::cout << ss.str();
            std::cout << std::endl << bc::right_top;
            for (size_t i = 0; i < string_actual_printed_length(ss.str()) - 2; ++i)
                std::cout << bc::h_line;
            std::cout << bc::left_top;

        }
    }
    ~Cirtular_list() {
        if (head_) {
            for (Node* node = head_->next_; node && node != head_; ) {
                Node* previous = node;
                node = node->next_;
                delete previous;
            }
            delete head_;
        }
    }
};

struct Node {
    Node* next_;
    int data_;
    Node(Node* next, int data) :next_(next), data_(data) {}
};

void print_forward_list(Node* head) {
    for (Node* node = head; node; node = node->next_)
        std::cout << node->data_ << " ";
    std::cout << std::endl;

}

void delete_forward_list(Node* head) {
    for (Node* node = head; node; ) {
        Node* previous = node;
        node = node->next_;
        delete previous;
    }
}

void josephus_problem() {
    std::cout << "Josephus problem" << std::endl;
    Node* head = new Node(nullptr, 10);
    Node* node = head;
    for (int i = 11; i < 30; ++i)
        node = (node->next_ = new Node(nullptr, i));
    node->next_ = head;

    auto print_circle = [](Node* head, std::ostream& stream) {
        stream << head->data_;
        for (Node* node = head->next_; node != head; node = node->next_)
            stream << " " << node->data_;
    };

    node = head;
    while (node != node->next_) {
        for (int i = 0; i < 5; ++i) node = node->next_;
        Node* to_remove = node->next_;
        node->next_ = node->next_->next_;
        std::cout << "[" << to_remove->data_ << "] ";
        std::stringstream ss;
        print_circle(node->next_, ss);
        std::cout << ss.str() << std::endl;
        delete to_remove;
    }
    std::cout << "[" << node->data_ << "]" << std::endl;

    delete node;
}

void list_reversal() {
    std::cout << "reversing list" << std::endl;
    Node* head = new Node(nullptr, 10);
    Node* node = head;
    for (int i = 11; i < 21; ++i)
        node = (node->next_ = new Node(nullptr, i));
    print_forward_list(head);
    {
        Node* previous = nullptr;
        for (Node* current = head; current; ) {
            Node* next = current->next_;
            current->next_ = previous;
            previous = current;
            current = next;
        }
        head = previous;
    }
    print_forward_list(head);
    delete_forward_list(head);
}

template<typename It>
void list_insertion_sort(It begin, It end) {
    auto it = begin;
    if (it == end) return;
    ++it;
    if (it == end) return;

    it = begin;
    Node* head = new Node(nullptr, *begin);
    for (++it; it != end; ++it) {
        if (*it < head->data_)
            head = new Node(head, *it);
        else {
            Node* node;
            for (node = head; node->next_; node = node->next_)
                if (*it < node->next_->data_)
                    break;
            node->next_ = new Node(node->next_, *it);
        }
    }
    print_forward_list(head);
    delete_forward_list(head);
}

int main() {
    Random_sequence_generator generator(300, 10, 99);

    Array<Entry> array(15);
    Forward_list<Entry> list;
    for (auto& e : array) {
        int value = generator.generate();
        e.value_ = value;
        list.emplace_back(value, false);
    }

    std::cout << "array sort" << std::endl;
    buble_sort(array.begin(), array.end());

    std::cout << "forward list sort" << std::endl;
    buble_sort(list.begin(), list.end());

    Array<int> ints(array.size());
    for (size_t i = 0; i < array.size(); ++i)
        ints[i] = array[i].value_;
    std::cout << "list insertion_sort" << std::endl;
    list_insertion_sort(ints.begin(), ints.end());

    std::cout << "array sequential search" << std::endl;
    clear_rich_text_format(array.begin(), array.end());
    sequential_search(array.cbegin(), array.cend(), 58);

    std::cout << "list sequential search" << std::endl;
    clear_rich_text_format(list.begin(), list.end());
    sequential_search(list.begin(), list.end(), 58);

    std::cout << "array quick search" << std::endl;
    auto do_quick_search = [&array](int value) {
        clear_rich_text_format(array.begin(), array.end());
        quick_search(array, value);
    };
    do_quick_search(58);
    do_quick_search(12);
    do_quick_search(100);

    eratosthenes_sieve(100);

    coin_flipping_simulation();

    Array<Point> points(200);
    {
        auto random = []() {
            return ((float) rand()) / RAND_MAX;
        };
        for (auto& point : points) {
            point.x_ = random();
            point.y_ = random();
        }
    }
    count_points_within_distance(points, 0.015);

    Cirtular_list c_list;
    c_list.add(1);
    c_list.add(2);
    c_list.add(3);
    c_list.print();
    std::cout << std::endl;

    josephus_problem();
    list_reversal();
}
