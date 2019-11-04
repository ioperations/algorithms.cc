#include <iostream>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "pair.h"

template<typename T>
class Rich_text {
    public:
        T value_;
        bool bold_;
        Rich_text() = default;
        template<typename TT>
            Rich_text(TT&& value, bool bold): value_(std::forward<TT>(value)), bold_(bold) {}
        template<typename TT>
            friend std::ostream& operator<<(std::ostream& s, Rich_text<TT> r) {
#ifdef unix
                if (r.bold_)
                    s << "\x1B[1m";
#endif
                s << r.value_;
#ifdef unix
                if (r.bold_)
                    s << "\x1B[0m";
#endif
                return s;
            }
};

using Connections = Array<Rich_text<int>>;
using Connection_pairs = Forward_list<Pair<int, int>>;

template<typename A>
struct Searcher : A {
    template<typename... Args>
        Searcher(Args&&... args) :A(std::forward<Args>(args)...) {}
    void search(const Connection_pairs& pairs, Connections& connections) {
        for (size_t i = 0; i < connections.size(); ++i) {
            connections[i].value_ = i;
            connections[i].bold_ = false;
        }
        std::cout << connections << std::endl;
        for (auto pair = pairs.cbegin(); pair != pairs.cend(); ++pair) {
            if (pair != pairs.cbegin())
                for (auto& connection : connections)
                    connection.bold_ = false;
            auto already_existing = A::add(pair->first_, pair->second_, connections);
            std::cout << connections << "  " << pair->first_ << "-" << pair->second_;
            if (!already_existing.empty()) {
                std::cout << "  ";
                for (auto it = already_existing.cbegin(); it != already_existing.cend(); ++it) {
                    if (it != already_existing.cbegin())
                        std::cout << "-";
                    std::cout << (*it);
                }

            }
            std::cout << std::endl;
        }
    }
};

struct Quick_find {
    auto add(int f, int s, Connections& connections) {
        Forward_list<int> l;
        auto c = connections[f].value_;
        if (c == connections[s].value_) {
            for (size_t i = 0; i < connections.size(); ++i)
                if (connections[i].value_ == c) 
                    l.push_back(i);
        } else 
            for (auto& connection : connections)
                if (connection.value_ == c) {
                    connection = connections[s];
                    connection.bold_ = true;
                }
        return l;
    };
};

template<typename A>
struct Quick_union : A {
    template<typename... Args>
        Quick_union(Args&&... args) :A(std::forward<Args>(args)...) {}
    auto add(int f, int s, Connections& connections) {
        Forward_list<int> l;
        auto follow_links = [&l, &connections](int& index) {
            bool linked;
            do {
                linked = index != connections[index].value_;
                if (linked)
                    index = connections[index].value_;
                connections[index].bold_ = true;
                l.push_back(index);
            } while(linked);
        };
        follow_links(f);
        follow_links(s);
        A::add(f, s, connections, l);
        return l;
    }
};

struct Quick_union_simple_adder {
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        if (f != s) {
            connections[f].value_ = s;
            l.clear();
        }
    }
};

struct Quick_union_weighted_adder {
    Array<int> sizes_;
    Quick_union_weighted_adder(size_t size) :sizes_(size) {}
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        if (f != s) {
            if (sizes_[f] < sizes_[s]) {
                connections[f].value_ = s; sizes_[s] += sizes_[f];
            } else {
                connections[s].value_ = f; sizes_[f] += sizes_[s];
            }
            l.clear();
        }
    }
};

int main() {

    auto input = R"(
3 4
4 9
8 0
2 3
5 6
2 9
5 9
7 3
4 8
5 6
0 2
6 1
q)";
    std::stringstream ss(input);

    Connection_pairs pairs;
    int size = 0;

    char cf, cs = '\0';
    do {
        ss >> cf;
        if (cs != '\0') {
            int f = cf - '0';
            int s = cs - '0';
            pairs.emplace_back(f, s);
            ++size;
            cs = '\0';
        } else
            cs = cf;
    } while (cf != 'q');

    std::cout << "connections to add:" << std::endl << pairs << std::endl;

    Connections connections(size);

    std::cout << "quick find:" << std::endl;
    Searcher<Quick_find>().search(pairs, connections);

    std::cout << "quick union:" << std::endl;
    Searcher<Quick_union<Quick_union_simple_adder>>().search(pairs, connections);

    std::cout << "weighted quick union:" << std::endl;
    Searcher<Quick_union<Quick_union_weighted_adder>>(connections.size()).search(pairs, connections);

}
