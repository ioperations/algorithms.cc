#include <iostream>
#include <sstream>
#include <map>

#include "array.h"
#include "forward_list.h"
#include "pair.h"
#include "tree.h"
#include "string_utils.h"
#include "rich_text.h"

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
        A::post_process(connections);
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
    void post_process(Connections& connections) {}
};

using Pair_tree_node = Forward_list_tree_node<Pair<int, int>>;

struct Tree_stringifier {
    std::string operator()(const Pair_tree_node& node) {
        std::stringstream ss;
        ss << node.value().first_;
        return ss.str();
    }
};
auto pair_tree_printer = Tree_printer<Pair_tree_node, Tree_stringifier>();

template<typename A>
struct Quick_union : A {

    using Lines = Forward_list<std::string>;
    struct Text_block {
        Lines lines_;
        size_t width_ = 0;
        int lines_count_ = 0;
        Text_block(Lines&& lines): lines_(std::move(lines)) {
            for (auto l : lines_) {
                width_ = std::max<size_t>(width_, string_length(l));
                ++lines_count_;
            }
        }
    };
    using Text_blocks = Forward_list<Text_block>;

    template<typename... Args>
        Quick_union(Args&&... args) :A(std::forward<Args>(args)...) {}

    auto add(int f, int s, Connections& connections) {
        Forward_list<int> l;
        auto follow_links = [&l, &connections](int& index) {
            bool linked;
            do {
                linked = index != connections[index].value_;
                if (linked) {
                    index = connections[connections[index].value_].value_;
                    // index = connections[index].value_;
                }
                connections[index].bold_ = true;
                l.push_back(index);
            } while(linked);
        };
        follow_links(f);
        follow_links(s);
        if (f != s)
            A::add(f, s, connections, l);
        return l;
    }

    void post_process(Connections& connections) {
        Array<Pair_tree_node> nodes(connections.size());
        for (size_t i = 0; i < connections.size(); ++i)
            nodes[i] = Pair_tree_node(Pair<int, int>(i, connections[i].value_)); // todo add emplace

        std::map<int, Pair_tree_node*> map;
        for (size_t i = 0; i < nodes.size(); ++i)
            map[i] = &nodes[i];

        for (auto& node : nodes) {
            auto id = node.value().first_;
            auto parent_id = node.value().second_;
            if (id != parent_id) {
                auto parent = map[parent_id];
                auto& children = parent->children();
                children.push_back(std::move(node));
                map[id] = &children.back();
            }
        }
        Text_blocks blocks;
        Forward_list<Lines::iterator> its;
        int lines_count = 0;
        for (auto& e : map)
            if (e.first == e.second->value().second_) {
                blocks.emplace_back(pair_tree_printer.compose_text_lines(*e.second));
                auto& block = blocks.back();
                lines_count = std::max(lines_count, block.lines_count_);
                its.push_back(block.lines_.begin());
            }
        for (int line = 0; line < lines_count; ++line) {
            auto block = blocks.begin();
            for (auto& entry : its) {
                if (!entry.empty()) {
                    std::cout << (*entry);
                    for (size_t i = 0; i < block->width_ - string_length(*entry); ++i)
                        std::cout << " ";
                    ++entry;
                } else {
                    for (size_t i = 0; i < block->width_; ++i)
                        std::cout << " ";
                }
                std::cout << "   ";
                ++block;
            }
            std::cout << std::endl;
        }

    }

};

struct Quick_union_simple_adder {
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        connections[f].value_ = s;
        l.clear();
    }
};

struct Quick_union_weighted_adder {
    Array<int> sizes_;
    Quick_union_weighted_adder(size_t size) :sizes_(size) {
        for (auto& size: sizes_)
            size = 0;
    }
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        if (sizes_[f] < sizes_[s]) {
            connections[f].value_ = s; sizes_[s] += sizes_[f];
        } else {
            connections[s].value_ = f; sizes_[f] += sizes_[s];
        }
        l.clear();
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
