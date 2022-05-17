#pragma once

#include <iostream>

#include "binary_tree.h"
#include "stack.h"

template <typename K, typename V>
class Map {
   public:
    struct Node {
        K key_;
        V value_;
    };
    using node_type = Binary_tree_node<Node>;

   private:
    template <typename KK, typename VV>
    friend class Map_printer;
    node_type* root_;

   public:
    Map() : root_(nullptr) {}
    ~Map() { delete root_; }
    void insert(const K& key, const V& value) {
        if (!root_) {
            root_ = new node_type({key, value});
            return;
        }
        auto node = root_;
        bool found = false;
        for (auto next = node; next && !found;) {
            if (key == node->value_.key_) {
                node->value_.value_ = value;
                found = true;
            } else {
                next = key < node->value_.key_ ? node->l_ : node->r_;
                if (next) node = next;
            }
        }
        if (!found) {
            auto n = new node_type({key, value}, nullptr, nullptr);
            if (key < node->value_.key_)
                node->l_ = n;
            else
                node->r_ = n;
        }
    }

    void rotate_right(node_type*& node) {
        auto x = node->l_;
        node->l_ = x->r_;
        x->r_ = node;
        node = x;
    }

    void rotate_left(node_type*& node) {
        auto x = node->r_;
        node->r_ = x->l_;
        x->l_ = node;
        node = x;
    }

    void insert_root(node_type*& node, const K& key, const V& value) {
        if (!node) {
            node = new node_type({key, value});
            return;
        }
        if (key == node->value_.key_) {
            node->value_.value_ = value;
        } else if (key < node->value_.key_) {
            insert_root(node->l_, key, value);
            rotate_right(node);
        } else {
            insert_root(node->r_, key, value);
            rotate_left(node);
        }
    }

    void insert_root(const K& key, const V& value) {
        insert_root(root_, key, value);
    }

    void iterate(node_type* node) {
        if (!node) return;
        iterate(node->l_);
        std::cout << node->value_.key_ << std::endl;
        iterate(node->r_);
    }

    void iterate() { iterate(root_); }
};

template <typename K, typename V>
class Map_printer_node_handler
    : public Binary_tree_printer_node_handler<typename Map<K, V>::Node> {
   public:
    std::string node_to_string(const typename Map<K, V>::node_type& n) {
        std::stringstream ss;
        ss << n.value().key_ << ":" << n.value().value_;
        return ss.str();
    }
};

template <typename K, typename V>
class Map_printer : public Tree_printer<typename Map<K, V>::node_type,
                                        Map_printer_node_handler<K, V>> {
   private:
    using Base = Tree_printer<typename Map<K, V>::node_type,
                              Map_printer_node_handler<K, V>>;

   public:
    void print(const Map<K, V>& map, std::ostream& stream) {
        Base::print(*map.root_, stream);
    }
};
