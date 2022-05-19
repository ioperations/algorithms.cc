#pragma once

#include <iostream>

#include "binary_tree.h"
#include "stack.h"

template <typename K, typename V>
class Map {
   public:
    struct Node {
        K m_key;
        V m_value;
    };
    using node_type = BinaryTreeNode<Node>;

   private:
    template <typename KK, typename VV>
    friend class MapPrinter;
    node_type* m_root;

   public:
    Map() : m_root(nullptr) {}
    ~Map() { delete m_root; }
    void insert(const K& key, const V& value) {
        if (!m_root) {
            m_root = new node_type({key, value});
            return;
        }
        auto node = m_root;
        bool found = false;
        for (auto next = node; next && !found;) {
            if (key == node->m_value.m_key) {
                node->m_value.m_value = value;
                found = true;
            } else {
                next = key < node->m_value.m_key ? node->m_l : node->m_r;
                if (next) node = next;
            }
        }
        if (!found) {
            auto n = new node_type({key, value}, nullptr, nullptr);
            if (key < node->m_value.m_key)
                node->m_l = n;
            else
                node->m_r = n;
        }
    }

    void rotate_right(node_type*& node) {
        auto x = node->m_l;
        node->m_l = x->m_r;
        x->m_r = node;
        node = x;
    }

    void rotate_left(node_type*& node) {
        auto x = node->m_r;
        node->m_r = x->m_l;
        x->m_l = node;
        node = x;
    }

    void insert_root(node_type*& node, const K& key, const V& value) {
        if (!node) {
            node = new node_type({key, value});
            return;
        }
        if (key == node->m_value.m_key) {
            node->m_value.m_value = value;
        } else if (key < node->m_value.m_key) {
            insert_root(node->m_l, key, value);
            rotate_right(node);
        } else {
            insert_root(node->m_r, key, value);
            rotate_left(node);
        }
    }

    void insert_root(const K& key, const V& value) {
        insert_root(m_root, key, value);
    }

    void iterate(node_type* node) {
        if (!node) return;
        iterate(node->m_l);
        std::cout << node->m_value.m_key << std::endl;
        iterate(node->m_r);
    }

    void iterate() { iterate(m_root); }
};

template <typename K, typename V>
class MapPrinterNodeHandler
    : public BinaryTreePrinterNodeHandler<typename Map<K, V>::Node> {
   public:
    std::string node_to_string(const typename Map<K, V>::node_type& n) {
        std::stringstream ss;
        ss << n.value().m_key << ":" << n.value().m_value;
        return ss.str();
    }
};

template <typename K, typename V>
class MapPrinter : public TreePrinter<typename Map<K, V>::node_type,
                                      MapPrinterNodeHandler<K, V>> {
   private:
    using Base =
        TreePrinter<typename Map<K, V>::node_type, MapPrinterNodeHandler<K, V>>;

   public:
    void print(const Map<K, V>& map, std::ostream& stream) {
        Base::print(*map.m_root, stream);
    }
};
