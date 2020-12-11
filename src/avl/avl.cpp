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
    int32_t node_b = node->balance;
    std::shared_ptr<tree_node> right_son = node->right;
    int32_t rs_b = right_son->balance;
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

    if (node_b == -2 && rs_b == -1) {
        node->balance = 0;
        right_son->balance = 0;
    } else if (node_b == -2 && rs_b == 0) {
        node->balance = -1;
        right_son->balance = 1;
    }
}

void avl_tree::_right_rotate(std::shared_ptr<tree_node> node) {
    int32_t node_b = node->balance;
    std::shared_ptr<tree_node> left_son = node->left;
    int32_t ls_b = left_son->balance;
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

    if (node_b == 2 && ls_b == 1) {
        node->balance = 0;
        left_son->balance = 0;
    } else if (node_b == 2 && ls_b == 0) {
        node->balance = 1;
        left_son->balance = -1;
    }
}

std::shared_ptr<tree_node> avl_tree::_rebalance(std::shared_ptr<tree_node> node) {
    if (node->balance == -2) {
        if(node->right->balance > 0) {
            _right_rotate(node->right);
        }
        _left_rotate(node);
    } else {
        if(node->left->balance < 0)
            _left_rotate(node->left);
        _right_rotate(node);
    }
    return node->parent;
}

void avl_tree::_go_up(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev) {
    if (prev == node->left) {
        node->balance++;
    } else {
        node->balance--;
    }
    if (node->balance == 0) {
        return;
    } else if (std::abs(node->balance) == 1) {
        if (node->parent == nullptr) {
            return;
        }
        _go_up(node->parent, node);
    } else {
        node = _rebalance(node);
        if (node->balance == 0 || node->parent == nullptr) {
            return;
        }
        _go_up(node->parent, node);
    }
}

bool avl_tree::insert(int32_t pid) {
    if (_root == nullptr) {
        _root = std::shared_ptr<tree_node>(new tree_node(pid));
        return true;
    } else {
        return _insert(pid, _root);
    }
}

bool avl_tree::_insert(int32_t pid, std::shared_ptr<tree_node> node) {
    if (pid == node->pid) {
        return false;
    } else if (pid < node->pid) {
        if (node->left == nullptr) {
            node->left = std::shared_ptr<tree_node>(new tree_node(pid));
            node->left->parent = node;
            _go_up(node, node->left);
            return true;
        } else {
            return _insert(pid, node->left);
        }
    } else {
        if (node->right == nullptr) {
            node->right = std::shared_ptr<tree_node>(new tree_node(pid));
            node->right->parent = node;
            _go_up(node, node->right);
            return true;
        } else {
            return _insert(pid, node->right);
        }
    }
}

void avl_tree::print() {
    _print("", _root, false);
}

void avl_tree::_print(const std::string& prefix, std::shared_ptr<tree_node> node, bool is_left) {
    if( node != nullptr ) {
        std::cout << prefix;
        std::cout << (is_left ? "├──" : "└──" );
        std::cout << node->pid << "\n";
        _print(prefix + (is_left ? "│   " : "    "), node->left, true);
        _print(prefix + (is_left ? "│   " : "    "), node->right, false);
    }    
}
