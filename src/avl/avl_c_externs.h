#pragma once
#include <stdbool.h>

struct avl_tree;
typedef struct avl_tree avl_tree;

bool    init_avl(avl_tree** tree);
bool    deinit_avl(avl_tree* tree);
bool    add_to_tree(avl_tree* tree, int id);
bool    remove_from_tree(avl_tree* tree, int id);
int     get_parent_id(avl_tree* tree, int id);
bool    get_path(avl_tree* tree, int id, int* path_len, int* path);
void    print_tree(avl_tree* tree);
int     get_root_pid(avl_tree* tree);
