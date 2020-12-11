#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <cstdint>

struct tree_node {
    int32_t pid;
    int32_t height;
    std::shared_ptr<tree_node> parent;
    std::shared_ptr<tree_node> left;
    std::shared_ptr<tree_node> right;

    tree_node(): pid(-1), height(0), parent(nullptr), left(nullptr), right(nullptr) {}
    tree_node(int32_t pid): pid(pid), height(0), parent(nullptr), left(nullptr), right(nullptr) {}
    ~tree_node() = default;
};

class avl_tree {
    private:
        std::shared_ptr<tree_node> _root;
        void _search(int32_t pid, std::vector<int32_t>& path, std::shared_ptr<tree_node> node);
        bool _insert(int32_t pid, std::shared_ptr<tree_node> node);
        void _remove(std::shared_ptr<tree_node> node);
        void _left_rotate(std::shared_ptr<tree_node> node);
        void _right_rotate(std::shared_ptr<tree_node> node);
        void _delete_sub_tree(std::shared_ptr<tree_node> node);

    public:
        avl_tree(): _root(nullptr) {};
        std::vector<int32_t> search(int32_t pid);
        bool insert(int32_t pid);
        bool remove(int32_t pid);
        ~avl_tree();
};

 