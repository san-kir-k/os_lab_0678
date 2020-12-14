#include "avl_wrapper.hpp"

#define MAX_PATH_LEN 48

static int buffer[48];

bool init_avl(avl_tree** tree) {
    *tree = new avl_tree();
    return true;
}

bool deinit_avl(avl_tree* tree) {
    delete tree;
    return true;
}

bool add_to_tree(avl_tree* tree, int id) {
    return tree->insert(id);
}

bool remove_from_tree(avl_tree* tree, int id) {
    return tree->remove(id);
}

int get_parent_id(avl_tree* tree, int id) {
    return tree->get_parent_pid(id);
}

bool get_path(avl_tree* tree, int id, int* path_len, int* path) {
    if (!tree->search(id, buffer, path_len)) {
        *path_len = -1;
        return false;
    }
    path = buffer;
    return true;
}

void print_tree(struct avl_tree* tree) {
    tree->print();
}

int get_root_pid(avl_tree* tree) {
    return tree->get_root_pid();
}
