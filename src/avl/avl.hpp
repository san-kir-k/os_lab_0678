#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <cstdint>

struct tree_node {
    int32_t pid;
    int32_t balance;
    std::shared_ptr<tree_node> parent;
    std::shared_ptr<tree_node> left;
    std::shared_ptr<tree_node> right;

    tree_node(): pid(-1), balance(0), parent(nullptr), left(nullptr), right(nullptr) {}
    tree_node(int32_t pid): pid(pid), balance(0), parent(nullptr), left(nullptr), right(nullptr) {}
    ~tree_node() = default;
};

class avl_tree {
    private:
        std::shared_ptr<tree_node> _root;
        std::shared_ptr<tree_node> _find(int32_t pid, std::shared_ptr<tree_node> node);
        void _search(int32_t pid, std::vector<int32_t>& path, std::shared_ptr<tree_node> node);
        bool _insert(int32_t pid, std::shared_ptr<tree_node> node);
        void _left_rotate(std::shared_ptr<tree_node> node);
        void _right_rotate(std::shared_ptr<tree_node> node);
        std::shared_ptr<tree_node> _rebalance(std::shared_ptr<tree_node> node);
        void _delete_sub_tree(std::shared_ptr<tree_node> node);
        void _rec_reconstruct(std::shared_ptr<tree_node>& new_root, std::shared_ptr<tree_node> node);
        void _reconstruct();
        void _go_up(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev);
        void _print(const std::string& prefix, std::shared_ptr<tree_node> node, bool is_left, int32_t height);
    public:
        avl_tree(): _root(nullptr) {};
        std::vector<int32_t> search(int32_t pid);
        bool insert(int32_t pid);
        void print();
        void delete_sub_tree(int32_t pid);
        ~avl_tree() = default;
};
 