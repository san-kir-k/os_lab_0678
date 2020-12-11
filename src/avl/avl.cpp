#include "avl.hpp"

std::vector<int32_t> avl_tree::search(int32_t pid) {
    std::vector<int32_t> path;
    _search(pid, path, _root);
    return path;
}
void avl_tree::_search(int32_t pid, std::vector<int32_t>& path, std::shared_ptr<tree_node> node) {
    if (node == nullptr) {
        path.clear();
        return;
    } else if (pid == node->pid) {
        path.push_back(node->pid);
        return;
    } else {
        path.push_back(node->pid);
        std::shared_ptr<tree_node> to = pid < node->pid ? node->left : node->right;
        return _search(pid, path, to);
    }
}

void avl_tree::_left_rotate(std::shared_ptr<tree_node> node) {
    std::shared_ptr<tree_node> right_son = node->right;
    if (right_son == nullptr) {
        return;
    }
    node->right = right_son->left;
    if (right_son->left != nullptr) {
        right_son->left->parent = node;
    }
    right_son->parent = node->parent;
    if (node->parent == nullptr) {
        _root = right_son;
    } else if (node == node->parent->left) {
        node->parent->left = right_son;
    } else {
        node->parent->right = right_son;
    }
    right_son->left = node;
    node->parent = right_son; 
}

void avl_tree::_right_rotate(std::shared_ptr<tree_node> node) {
    std::shared_ptr<tree_node> left_son = node->left;
    if (left_son == nullptr) {
        return;
    }
    node->left = left_son->right;
    if (left_son->right != nullptr) {
        left_son->right->parent = node;
    }
    left_son->parent = node->parent;
    if (node->parent == nullptr) {
        _root = left_son;
    } else if (node == node->parent->right) {
        node->parent->right = left_son;
    } else {
        node->parent->left = left_son;
    }
    left_son->right = node;
    node->parent = left_son; 
}