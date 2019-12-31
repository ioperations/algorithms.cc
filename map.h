#pragma once

#include "binary_tree.h"
#include "stack.h"

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
            if (!root_) {
                root_ = new node_type({key, value});
                return;
            }
            auto node = root_;
            bool found = false;
            for (auto next = node; next && !found; ) {
                if (key == node->value_.key_) {
                    node->value_.value_ = value;
                    found = true;
                } else {
                    next = key < node->value_.key_ ? node->l_ : node->r_;
                    if (next)
                        node = next;
                }
            }
            if (!found) {
                if (key < node->value_.key_)
                    node->l_ = new node_type({key, value});
                else
                    node->r_ = new node_type({key, value});
            }
        }

        void iterate(node_type* node) {
            if (!node)
                return;
            iterate(node->l_);
            std::cout << node->value_.key_ << std::endl;
            iterate(node->r_);
        }

        void iterate() {
            iterate(root_);
        }
};

template<typename K, typename V>
class Map_printer_node_handler : public Binary_tree_printer_node_handler<typename Map<K, V>::Node> {
    public:
        std::string node_to_string(const typename Map<K, V>::node_type& n) {
            std::stringstream ss;
            ss << n.value().key_ << ":" << n.value().value_;
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
