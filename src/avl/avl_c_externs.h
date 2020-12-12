#pragma once

struct avl_tree;

bool init_avl(struct avl_tree** tree);

bool deinit_avl(struct avl_tree* tree);

bool add_to_tree(struct avl_tree* tree, int id);

bool remove_from_tree(struct avl_tree* tree, int id);

int get_parent_id(struct avl_tree* tree, int id);

int* get_path(struct avl_tree* tree, int id, int* path_len);
