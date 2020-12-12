#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include <string>
#include <memory>
#include <cstdint>

/**
 * структура для процесса-вершины дерева с укаанным id
 */
struct tree_node {
    int32_t pid;
    int32_t balance;
    std::weak_ptr<tree_node> parent;
    std::shared_ptr<tree_node> left;
    std::shared_ptr<tree_node> right;

    tree_node(): pid(-1), balance(0), left(nullptr), right(nullptr) {
        parent.lock() = nullptr;
    }
    tree_node(int32_t pid): pid(pid), balance(0), left(nullptr), right(nullptr) {
        parent.lock() = nullptr;
    }
    ~tree_node() = default;
};

class avl_tree {
    private:
        // корень дерева
        std::shared_ptr<tree_node> _root;
        // рекурсивный поиск по дереву, возвращает указатель на искомый узел или nullptr
        std::shared_ptr<tree_node> _find(int32_t pid, std::shared_ptr<tree_node> node);
        // рекурсивынй поиск с обновлением пути
        bool _search(int32_t pid, int32_t* path, std::shared_ptr<tree_node> node, int32_t* height);
        // рекурсивная функция для вставки
        bool _insert(int32_t pid, std::shared_ptr<tree_node> node);
        // левый поворот
        void _left_rotate(std::shared_ptr<tree_node> node);
        // правый поворот
        void _right_rotate(std::shared_ptr<tree_node> node);
        // перебалансировка вершины с балансом равным 2 по модулю
        std::shared_ptr<tree_node> _rebalance(std::shared_ptr<tree_node> node);
        // удаление всего поддерева с корнем в вершине node
        void _delete_sub_tree(std::shared_ptr<tree_node> node);
        // рекурсивно перестраиваем дерево после удаления целого поддерева
        void _rec_reconstruct(std::shared_ptr<tree_node>& new_root, std::shared_ptr<tree_node> node);
        // функция, вызывающая _rec_reconstruct, она обновляет корень
        void _reconstruct();
        // рекурсивное обновление баланса для вставки
        void _go_up_insert(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev);
        // вывод дерева на экран рекурсивно
        void _print(const std::string& prefix, std::shared_ptr<tree_node> node, bool is_left, int32_t height);
        // поиск и удаленеи вершины
        void _remove(std::shared_ptr<tree_node> node);
        // рекурсивное обновление баланса для удаления
        void _go_up_remove(std::shared_ptr<tree_node> node, std::shared_ptr<tree_node> prev);
    public:
        /**
         * конструктор по умолчанию
         */
        avl_tree(): _root(nullptr) {};
        /**
         * поиск в дереве по id процесса
         * возвращает вектор-путь от корневого процесса до искомого
         * если искомый процесс не найден, то возвращаемый вектор имеет длину 0
         */
        bool search(int32_t pid, int32_t* path, int32_t* path_len);
        /**
         * вставка в дерево по id процесса
         * в случае успеха вернет true
         * в случае неуспеха вернет false
         */
        bool insert(int32_t pid);
        /** 
         * печать дерева на экран
         */
        void print();
        /**
         * удаление всего поддерва, корнем которого является процесс с указанным id
         * в случае успеха дерево перебалансируется
         * в случае, если такого процесса нет, то метод вернет false
         */
        bool delete_sub_tree(int32_t pid);
        /**
         * удаление процесса с номером pid
         * в случае неуспеха вернет false
         */
        bool remove(int32_t pid);
        /**
         * получить pid родителя
         * в случае неуспеха вернется -1
         */
        int32_t get_parent_pid(int32_t pid);
        /** 
         * деструктор по умолчанию
         */
        ~avl_tree() = default;
};
