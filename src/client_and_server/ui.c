#include "ui.h"

void 
skip_str() {
    int c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

void
help() {
    printf("+--------------+-------------------------------------------+\n");
    printf("|   command    |               description                 |\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("| create <pid> |        creates computing node             |\n");
    printf("|              |       (pids range from 0 to 63)           |\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("| remove <pid> |         removes computing node            |\n");
    printf("|              |        (pids range from 0 to 63)          |\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("|  exec <pid>  |          exec computing node              |\n");
    printf("|    <text>    |         finds pattern in text             |\n");
    printf("|   <pattern>  |        (pids range from 0 to 63)          |\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("|     help     |              print usage                  |\n");
    printf("|              |       (pids range from 0 to 63)           |\n");
    printf("|--------------+-------------------------------------------|\n");
    printf("|     print    |         print process avl tree            |\n");
    printf("+--------------+-------------------------------------------+\n\n");
}

input_val
input_int(int32_t* val) {
    int32_t scanf_val;
    if ((scanf_val = scanf("%d", val)) == EOF) {
        return eof;
    } else if (scanf_val == 0) {
        skip_str();
        return bad;
    }
    return ok;
}

input_val
input_str(char* val) {
    int32_t scanf_val;
    if ((scanf_val = scanf("%s", val)) == EOF) {
        return eof;
    } else if (scanf_val == 0) {
        skip_str();
        return bad;
    }
    return ok;
}
