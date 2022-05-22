#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include "binary_tree.h"
#include "popl/popl.hpp"
#include "tree.h"
#include "tree_printer.h"

template <typename N>
auto to_string(const N& node) {
    std::stringstream ss;
    ss << std::endl;
    ss << node;
    return ss.str();
}

#include <iostream>
#include <optional>
#include <queue>
#include <vector>
using namespace std;

using TreeNode = BinaryTreeNode<int>;
// Decodes your encoded data to tree.
TreeNode* construct_binary_tree(std::vector<std::optional<int>>& data) {
    data.resize(data.size() * 3 + 31);
    if (data.size() == 0) return nullptr;

    if (!data[0].has_value()) return nullptr;
    TreeNode* root = new TreeNode(data[0].value());
    queue<TreeNode*> q;
    q.push(root);

    int i = 1;

    while (!q.empty()) {
        TreeNode* cur = q.front();
        q.pop();

        if (!data[i].has_value()) {
            cur->m_l = NULL;
        } else {
            TreeNode* left_n = new TreeNode(data[i].value());
            cur->m_l = left_n;
            q.push(left_n);
        }
        i++;

        if (!data[i].has_value()) {
            cur->m_r = NULL;
        } else {
            TreeNode* right_n = new TreeNode(data[i].value());
            cur->m_r = right_n;
            q.push(right_n);
        }
        i++;
    }
    return root;
}

// Function to print tree nodes in
// InOrder fashion
void in_order(TreeNode* root, std::vector<string>& vec) {
    if (root != nullptr) {
        in_order(root->m_l, vec);
        vec.push_back(std::to_string(root->m_value));

        in_order(root->m_r, vec);
    }
}

void bfs_search(TreeNode* root, std::vector<int>& vec) {
    queue<TreeNode*> q;
    q.push(root);

    while (q.size()) {
        TreeNode* tmp = q.front();

        q.pop();

        if (tmp != nullptr) {
            q.push(tmp->m_l);
            q.push(tmp->m_r);
            vec.push_back(tmp->m_value);
        }
    }
}

void free_tree_node(TreeNode* root) {
    if (root == nullptr) return;

    free_tree_node(root->m_l);
    free_tree_node(root->m_r);

    delete root;
}

void test(int begin, int end, int step) {
    vector<optional<int>> vec;
    for (int i = begin; i < end; i += step) {
        vec.push_back(i);
    }

    TreeNode* n = construct_binary_tree(vec);

    std::cout << to_string(*n) << std::endl;
}

#define MAX_SIZE 5000

int main(int argc, char* argv[]) {
    popl::OptionParser op("ptree");
    auto help = op.add<popl::Switch>("h", "help", "help message");
    auto begin = op.add<popl::Value<int>>("f", "from", "ptree start from ", 0);
    auto end = op.add<popl::Value<int>>("t", "to", "ptree end until");
    auto step = op.add<popl::Value<int>>("s", "step", "ptree step from ", 1);

    op.parse(argc, argv);
    if (help->is_set()) {
        std::cerr << op << std::endl;
        return -1;
    }
    if (end->is_set()) {
        if ((end->value() - begin->value()) && (step->value() == 0)) {
            std::cerr << "range from " << begin->value() << " to "
                      << end->value() << " ,but step is 0" << std::endl;
            return -1;
        }
        if ((end->value() - begin->value()) * step->value() < 0) {
            std::cerr << "cannot range from " << begin->value() << " to "
                      << end->value() << " while step is " << step->value()
                      << std::endl;
            return -1;
        }
        if ((end->value() - begin->value()) <= step->value() * MAX_SIZE) {
            test(begin->value(), end->value(), step->value());
        } else {
            std::cerr << "end value is too much big" << std::endl;
        }
    } else {
        std::cerr << "do not have end set" << std::endl;
        std::cerr << op << std::endl;
    }
    return 0;
}

