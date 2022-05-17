#include <iostream>
#include <map>
#include <sstream>

#include "array.h"
#include "forward_list.h"
#include "pair.h"
#include "rich_text.h"
#include "text_block.h"
#include "tree.h"

using Entry = Rich_text::Entry<int>;
using Style = Rich_text::Style;
using Connections = Array<Entry>;
using Connection_pairs = Forward_list<Pair<int, int>>;

void print_links(const Forward_list<int>& links, std::ostream& stream) {
    if (!links.empty()) {
        stream << "  ";
        for (auto it = links.cbegin(); it != links.cend(); ++it) {
            if (it != links.cbegin()) stream << "-";
            stream << (*it);
        }
    }
}

template <typename A>
struct Searcher : A {
    template <typename... Args>
    Searcher(Connections& connections, Args&&... args)
        : A(connections, std::forward<Args>(args)...) {}
    void search(const Connection_pairs& pairs) {
        for (size_t i = 0; i < A::m_connections.size(); ++i) {
            A::m_connections[i].value = i;
            A::m_connections[i].remove_styles();
        }
        for (auto pair = pairs.cbegin(); pair != pairs.cend(); ++pair) {
            if (pair != pairs.cbegin())
                for (auto& connection : A::m_connections)
                    connection.remove_styles();
            A::add(pair, pairs);
        }
    }
};

struct QuickFind {
    Connections& m_connections;
    QuickFind(Connections& connections) : m_connections(connections) {}
    void add(Connection_pairs::const_iterator pair,
             const Connection_pairs& pairs) {
        if (pair == pairs.cbegin()) std::cout << m_connections << std::endl;
        Forward_list<int> l;
        auto c = m_connections[pair->m_first].value;
        if (c == m_connections[pair->m_second].value) {
            for (size_t i = 0; i < m_connections.size(); ++i)
                if (m_connections[i].value == c) l.push_back(i);
        } else
            for (auto& connection : m_connections)
                if (connection.value == c) {
                    connection = m_connections[pair->m_second];
                    connection.add_style(Style::bold());
                }
        std::cout << m_connections << "  " << pair->m_first << "-"
                  << pair->m_second;
        print_links(l, std::cout);
        std::cout << std::endl;
    };
};

using Pair_tree_node = ForwardListTreeNode<Pair<Entry, int>>;

struct PairTreePrinterNodeHandler
    : public Tree_printer_node_handler<Pair_tree_node> {
    std::string node_to_string(const Pair_tree_node& node) {
        std::stringstream ss;
        ss << node.value().m_first;
        return ss.str();
    }
    size_t label_width(const Pair_tree_node& node, const std::string& label) {
        return string_actual_printed_length(label);
    }
};
auto pair_tree_printer =
    Tree_printer<Pair_tree_node, PairTreePrinterNodeHandler>();

template <typename A>
struct QuickUnion : A {
    struct CustomTextBlocks : public TextBlocks {
        size_t m_trees_width = 0;
        TextBlock* m_last_tree = nullptr;
    };

    Connections& m_connections;
    Forward_list<CustomTextBlocks> m_text_blocks_lines;

    template <typename... Args>
    QuickUnion(Connections& connections, Args&&... args)
        : A(std::forward<Args>(args)...), m_connections(connections) {}

    void add(Connection_pairs::const_iterator pair,
             const Connection_pairs& pairs) {
        int f = pair->m_first, s = pair->m_second, fi = f,
            si = s;  // todo remove
        Forward_list<int> l;
        auto follow_links = [&l, this](int& index) {
            bool linked;
            do {
                linked = index != m_connections[index].value;
                if (linked) {
                    index = m_connections[m_connections[index].value].value;
                    // index = connections_[index].value;
                }
                m_connections[index].add_style(Style::bold());
                l.push_back(index);
            } while (linked);
        };
        follow_links(f);
        follow_links(s);
        if (f != s) {
            A::add(f, s, m_connections, l);
        }

        CustomTextBlocks blocks;
        if (f != s) {
            Array<Pair_tree_node> nodes(m_connections.size());
            for (size_t i = 0; i < m_connections.size(); ++i) {
                auto style = (int)i == fi || (int)i == si ? Style::bold()
                                                          : Style::normal();
                nodes.emplace(i, Pair<Entry, int>(Entry(i, style),
                                                  m_connections[i].value));
            }

            std::map<int, Pair_tree_node*> map;
            for (size_t i = 0; i < nodes.size(); ++i) map[i] = &nodes[i];

            for (auto& node : nodes) {
                auto id = node.value().m_first.value;
                auto parent_id = node.value().m_second;
                if (id != parent_id) {
                    auto* parent = map[parent_id];
                    auto& children = parent->children();
                    children.push_back(std::move(node));
                    map[id] = &children.back();
                }
            }
            for (auto& e : map)
                if (e.first == e.second->value().m_second)
                    blocks.emplace_back(
                        pair_tree_printer.compose_text_lines(*e.second));
        } else
            blocks.emplace_back("(same)");
        blocks.m_trees_width = blocks.width();
        blocks.m_last_tree = &blocks.back();

        std::stringstream ss;
        ss << fi << "-" << si << "  ";
        print_links(l, ss);
        blocks.emplace_back(ss.str());
        m_text_blocks_lines.push_back(std::move(blocks));

        auto next_pair = pair;
        ++next_pair;
        if (next_pair == pairs.cend()) {
            size_t max_trees_width = 0;
            for (auto& blocks : m_text_blocks_lines)
                max_trees_width =
                    std::max(max_trees_width, blocks.m_trees_width);
            for (auto& blocks : m_text_blocks_lines) {
                auto last_tree = blocks.m_last_tree;
                last_tree->set_width(last_tree->width() + max_trees_width -
                                     blocks.m_trees_width);
                std::cout << blocks << std::endl;
            }
        }
    }
};

struct QuickUnionSimpleAdder {
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        connections[f].value = s;
        l.clear();
    }
};

struct QuickUnionWeightedAdder {
    Array<int> m_sizes;
    QuickUnionWeightedAdder(size_t size) : m_sizes(size) {
        for (auto& size : m_sizes) size = 0;
    }
    void add(int f, int s, Connections& connections, Forward_list<int>& l) {
        if (m_sizes[f] < m_sizes[s]) {
            connections[f].value = s;
            m_sizes[s] += m_sizes[f];
        } else {
            connections[s].value = f;
            m_sizes[f] += m_sizes[s];
        }
        l.clear();
    }
};

int main() {
    const auto* input = R"(
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
    Searcher<QuickFind>(connections).search(pairs);

    std::cout << "quick union:" << std::endl;
    Searcher<QuickUnion<QuickUnionSimpleAdder>>(connections).search(pairs);

    std::cout << "weighted quick union:" << std::endl;
    Searcher<QuickUnion<QuickUnionWeightedAdder>>(connections,
                                                  connections.size())
        .search(pairs);
}
