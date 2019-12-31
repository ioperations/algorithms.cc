#pragma once

#include "binary_tree.h"

#include <iostream>

template<typename K, typename V>
class Map {
    public:
        struct Node {
            K key_;
            V value_;
        };
        using node_type = Binary_tree_node<Node>;
    private:
        template<typename KK, typename VV>
        friend class Map_printer;
        node_type* root_;
    public:
        Map() :root_(nullptr) {}
        ~Map() { delete root_; }
        void insert(const K& key, const V& value) {
            if (root_) {
            } else {
                root_ = new Binary_tree_node<Node>({key, value});
            }
        }
};

template<typename K, typename V>
class Map_printer_node_handler : public Binary_tree_printer_node_handler<typename Map<K, V>::Node> {
    public:
        std::string node_to_string(const typename Map<K, V>::node_type& n) {
            std::stringstream ss;
            ss << "{" << n.value().key_ << ": " << n.value().value_ << "}";
            return ss.str();
        }
};

template<typename K, typename V>
class Map_printer : 
    public Tree_printer<typename Map<K, V>::node_type, Map_printer_node_handler<K, V>> {
        private:
            using Base = Tree_printer<typename Map<K, V>::node_type, Map_printer_node_handler<K, V>>;
        public:
            void print(const Map<K, V>& map, std::ostream& stream) {
                Base::print(*map.root_, stream);
            }
};
