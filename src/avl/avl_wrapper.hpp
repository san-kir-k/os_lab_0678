#pragma once

#include "avl.hpp"

extern "C" bool init_avl(avl_tree** tree);

extern "C" bool deinit_avl(avl_tree* tree);

extern "C" bool add_to_tree(avl_tree* tree, int id);

extern "C" bool remove_from_tree(avl_tree* tree, int id);

extern "C" int get_parent_id(avl_tree* tree, int id);

extern "C" bool get_path(avl_tree* tree, int id, int* path_len, int* path);

extern "C" void print_tree(avl_tree* tree);

extern "C" int get_root_pid(avl_tree* tree);
