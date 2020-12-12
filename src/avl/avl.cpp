#include "avl.hpp"

// поиск по дереву - начало
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
// поиск по дереву - конец

// повороты - начало
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
// повороты - конец

// перебалансировка - начало
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
// перебалансировка - конец

// вставка в дерево - начало
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
            _go_up_insert(node, node->left);
            return true;
        } else {
            return _insert(pid, node->left);
        }
    } else {
        if (node->right == nullptr) {
            node->right = std::shared_ptr<tree_node>(new tree_node(pid));
            node->right->parent = node;
            _go_up_insert(node, node->right);
            return true;
        } else {
            return _insert(pid, node->right);
        }
    }
}

void avl_tree::_go_up_insert(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev) {
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
        _go_up_insert(node->parent, node);
    } else {
        node = _rebalance(node);
        if (node->balance == 0 || node->parent == nullptr) {
            return;
        }
        _go_up_insert(node->parent, node);
    }
}

// вставка в дерево - конец

// вывод дерева на экран - начало
void avl_tree::print() {
    _print("", _root, false, 1);
}

void avl_tree::_print(const std::string& prefix, std::shared_ptr<tree_node> node, bool is_left, int32_t height) {
    if( node != nullptr ) {
        std::string new_prefix = "";
        for (int32_t i = 0; i < height; ++i) {
            new_prefix += "    ";
        }
        _print(new_prefix, node->left, true, height + 1);
        std::cout << prefix;
        if (height == 1) {
            std::cout << "───";
        } else {
            std::cout << (is_left ? "┌──" : "└──" );
        }
        std::cout << node->pid << "\n";
        _print(new_prefix, node->right, false, height + 1);
    }    
}
// вывод дерева на экран - конец

// удаление поддерева - начало
bool avl_tree::delete_sub_tree(int32_t pid) {
    std::shared_ptr<tree_node> to_delete = _find(pid, _root);
    if (to_delete == nullptr) {
        return false;
    }
    _delete_sub_tree(to_delete);
    _reconstruct();
    return true;
}

std::shared_ptr<tree_node> avl_tree::_find(int32_t pid, std::shared_ptr<tree_node> node) {
    if (node == nullptr) {
        return nullptr;
    } else if (pid == node->pid) {
        return node;
    } else {
        std::shared_ptr<tree_node> to = pid < node->pid ? node->left : node->right;
        return _find(pid, to);
    }
}

void avl_tree::_delete_sub_tree(std::shared_ptr<tree_node> node) {
    if (node == nullptr) {
        return;
    } else {
        if (node->parent->left == node) {
            node->parent->left = nullptr;
        } else {
            node->parent->right = nullptr;
        }
        node->parent = nullptr;
    }
}

void avl_tree::_reconstruct() {
    std::shared_ptr<tree_node> old_root = _root;
    _root = std::shared_ptr<tree_node>(new tree_node(old_root->pid));
    _rec_reconstruct(_root, old_root->left);
    _rec_reconstruct(_root, old_root->right);
}

void avl_tree::_rec_reconstruct(std::shared_ptr<tree_node>& new_root, std::shared_ptr<tree_node> node) {
    if (node == nullptr) {
        return;
    }
    _insert(node->pid, new_root);
    _rec_reconstruct(new_root, node->left);
    _rec_reconstruct(new_root, node->right);
}
// удаление поддерева - конец

// удаление вершины дерева - начало
bool avl_tree::remove(int32_t pid) {
    std::shared_ptr<tree_node> node = _root;
    while ((node != nullptr) && (pid != node->pid)) {
        std::shared_ptr<tree_node> to = (pid < node->pid) ? node->left : node->right;
        node = to;
    }
    if (node == nullptr) {
        return false;
    }
    _remove(node);
    return true;
}

void avl_tree::_remove(std::shared_ptr<tree_node> node) {
    std::shared_ptr<tree_node> to_delete = node;
    int32_t to_delete_balance = to_delete->balance;
    std::shared_ptr<tree_node> to_replace;
    std::shared_ptr<tree_node> to_replace_parent;
    if (node->left == nullptr) {
        to_replace = node->right;
        if (to_replace != nullptr) {
            to_replace->parent = node->parent;
            to_replace_parent = node->parent;
        } else {
            to_replace_parent = node->parent;
            if (node == _root) {
                to_replace_parent = nullptr;
                _root = nullptr;
            }
        }
        if (node->parent != nullptr) {
            if (node->parent->left == node) {
                node->parent->left = to_replace;
            } else {
                node->parent->right = to_replace;
            }
        } else {
            _root = to_replace;
        }
    } else if (node->right == nullptr) {
        to_replace = node->left;
        to_replace->parent = node->parent;
        to_replace_parent = node->parent;
        if (node->parent != nullptr) {
            if (node->parent->left == node) {
                node->parent->left = to_replace;
            } else {
                node->parent->right = to_replace;
            }
        } else {
            _root = to_replace;
        }           
    } else {
        std::shared_ptr<tree_node> min_in_right = node->right;
        while(min_in_right->left != nullptr) {
            min_in_right = min_in_right->left;
        }
        to_delete = min_in_right;
        to_delete_balance = to_delete->balance;
        to_replace = to_delete->right;
        if (to_delete->parent == node) {
            if (to_replace != nullptr) {
                to_replace->parent = to_delete;
            }
            to_replace_parent = to_delete;
        } else {
            to_delete->parent->left = to_replace;
            if (to_replace != nullptr) {
                to_replace->parent = to_delete->parent;
            }
            to_replace_parent = to_delete->parent;
            to_delete->right = node->right;
            to_delete->right->parent = to_delete;
        }
        if (node->parent != nullptr) {
            if (node->parent->left == node) {
                node->parent->left = to_delete;
            } else {
                node->parent->right = to_delete;
            }
        } else {
            _root = to_delete;
        }
        to_delete->parent = node->parent;
        to_delete->left = node->left;
        to_delete->left->parent = to_delete;
        to_delete->balance = node->balance;
    }
    _go_up_remove(to_replace_parent, to_replace);
}
 
void avl_tree::_go_up_remove(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev) {
    if (prev == node->left) {
        node->balance--;
    } else {
        node->balance++;
    }
    if (std::abs(node->balance) == 1) {
        return;
    } else if (node->balance == 0) {
        if (node->parent == nullptr) {
            return;
        }
        _go_up_remove(node->parent, node);
    } else {
        node = _rebalance(node);
        if (node->balance == 0 || node->parent == nullptr) {
            return;
        }
        _go_up_remove(node->parent, node);
    }
}
// удаление вершины дерева - конец
