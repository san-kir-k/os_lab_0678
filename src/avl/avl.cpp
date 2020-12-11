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