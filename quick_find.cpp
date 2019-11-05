#include <iostream>
#include <sstream>
#include <map>

#include "array.h"
#include "forward_list.h"
#include "pair.h"
#include "tree.h"
#include "rich_text.h"
#include "text_block.h"

using Connections = Array<Rich_text<int>>;
using Connection_pairs = Forward_list<Pair<int, int>>;

void print_links(const Forward_list<int>& links, std::ostream& stream) {
    if (!links.empty()) {
        stream << "  ";
        for (auto it = links.cbegin(); it != links.cend(); ++it) {
            if (it != links.cbegin())
                stream << "-";
            stream << (*it);
        }

    }
}

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
            // std::cout << connections << "  " << pair->first_ << "-" << pair->second_;
            // print_links(already_existing, std::cout);
            // std::cout << std::endl;
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

    struct Custom_text_blocks : public Text_blocks {
        size_t trees_width_ = 0;
        Text_block* last_tree_ = nullptr;
    };

    Forward_list<Custom_text_blocks> text_blocks_lines_;

    template<typename... Args>
        Quick_union(Args&&... args) :A(std::forward<Args>(args)...) {}

    auto add(int f, int s, Connections& connections) {
        int fi = f, si = s; // todo remove
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
        if (f != s) {
            A::add(f, s, connections, l);
        }

        Custom_text_blocks blocks;
        if (f != s) {
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
            for (auto& e : map)
                if (e.first == e.second->value().second_)
                    blocks.emplace_back(pair_tree_printer.compose_text_lines(*e.second));
        } else
            blocks.emplace_back("(same)");
        blocks.trees_width_ = blocks.width();
        blocks.last_tree_ = &blocks.back();

        std::stringstream ss;
        ss << fi << "-" << si << "  ";
        print_links(l, ss); 
        blocks.emplace_back(ss.str());
        text_blocks_lines_.push_back(std::move(blocks));
        return l;
    }

    void post_process(Connections& connections) {
        size_t max_trees_width = 0;
        for (auto& blocks : text_blocks_lines_)
            max_trees_width = std::max(max_trees_width, blocks.trees_width_);
        for (auto& blocks : text_blocks_lines_) {
            auto last_tree = blocks.last_tree_;
            last_tree->set_width(last_tree->width() + max_trees_width - blocks.trees_width_);
            std::cout << blocks << std::endl;
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

    // std::cout << "quick find:" << std::endl;
    // Searcher<Quick_find>().search(pairs, connections);

    // std::cout << "quick union:" << std::endl;
    // Searcher<Quick_union<Quick_union_simple_adder>>().search(pairs, connections);

    std::cout << "weighted quick union:" << std::endl;
    Searcher<Quick_union<Quick_union_weighted_adder>>(connections.size()).search(pairs, connections);

}
