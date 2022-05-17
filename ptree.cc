#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "binary_tree.h"
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

using TreeNode = Binary_tree_node<int>;
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
            cur->l_ = NULL;
        } else {
            TreeNode* left_n = new TreeNode(data[i].value());
            cur->l_ = left_n;
            q.push(left_n);
        }
        i++;

        if (!data[i].has_value()) {
            cur->r_ = NULL;
        } else {
            TreeNode* right_n = new TreeNode(data[i].value());
            cur->r_ = right_n;
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
        in_order(root->l_, vec);
        vec.push_back(std::to_string(root->value_));

        in_order(root->r_, vec);
    }
}

void bfs_search(TreeNode* root, std::vector<int>& vec) {
    queue<TreeNode*> q;
    q.push(root);

    while (q.size()) {
        TreeNode* tmp = q.front();

        q.pop();

        if (tmp != nullptr) {
            q.push(tmp->l_);
            q.push(tmp->r_);
            vec.push_back(tmp->value_);
        }
    }
}

void free_tree_node(TreeNode* root) {
    if (root == nullptr) return;

    free_tree_node(root->l_);
    free_tree_node(root->r_);

    delete root;
}

void test(int argc, char* argv[]) {
    std::optional<int> tmp;

    vector<optional<int>> vec;
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "null") == 0) {
            vec.push_back(tmp);
        } else {
            vec.push_back(atoi(argv[i]));
        }
    }

    TreeNode* n = construct_binary_tree(vec);

    std::cout << to_string(*n) << std::endl;

    //   free_tree_node(n);
    //
}
void usage(int argc, char* argv[]) { fprintf(stderr, "%s  1 2 3 4 ", argv[0]); }

int main(int argc, char* argv[]) {
    if (argc > 1 && (strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "-") == 0 ||
                     strcmp(argv[1], "--help") == 0)) {
        usage(argc, argv);
        return -1;
    }
    test(argc, argv);
    return 0;
}

