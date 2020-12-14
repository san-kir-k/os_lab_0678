#include <stdio.h>
#include <unistd.h>
#include <zmq.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <limits.h>

#include "../avl/avl_c_externs.h"
#include "ui.h"
#include "zmq_handle.h"
#include "defines.h"

static int32_t      REAL_PID_TABLE[MAX_PID_NUM];
static avl_tree*    AVL_TREE_PTR;
static void*        CONTEXT;
static void*        EXEC_PUB;
static void*        HEARTBIT_SUB;

void
init_table() {
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        REAL_PID_TABLE[i] = VOID_PID;
    }
}

bool
remove_node(int32_t pid) {
    if (pid < 0 || pid > 63) {
        printf("Invalid pid.\n");
        return false;
    }
    int32_t rpid = REAL_PID_TABLE[pid];
    if (rpid == VOID_PID) {
        printf("Invalid pid.\n");
        return false;
    }
    kill(rpid, SIGTERM);
    REAL_PID_TABLE[pid] = VOID_PID;
    remove_from_tree(AVL_TREE_PTR, pid);
    //...
    return true;
}

void
terminate_all_nodes() {
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        if (REAL_PID_TABLE[i] != VOID_PID) {
            remove_node(i);
        }
    }
}

bool
create_node(int32_t pid) {
    if (pid < 0 || pid > 63) {
        printf("Invalid pid.\n");
        return false;
    }
    if (REAL_PID_TABLE[pid] != VOID_PID) {
        printf("Node with this pid has already created.\n");
        return false;
    }
    int32_t fv = fork();
    if (fv < 0) {
        printf("Unable to create node, fork err.\n");
        return false;
    } else if (fv == 0) {
        char client_id[10];
        sprintf(client_id, "%d", pid);
        char client_name[20];
        sprintf(client_name, "client_%d", pid);
        char parent_name[64] = MASTER_SOCKET;
        execl(CLIENT_NAME, client_name, client_id, parent_name, NULL);
    } else {
        REAL_PID_TABLE[pid] = fv;
        add_to_tree(AVL_TREE_PTR, pid);
    }
    return true;
}

bool
send_exec(char* text, char* pattern) {
    zmq_msg_t message;
    zmq_msg_init(&message);
    zmq_msg_send(&message, EXEC_PUB, ZMQ_SNDMORE);
    zmq_msg_close(&message);
    return true;
}

void 
server_loop() {
    char cmd[32];
    int32_t pid = -1;
    while (true) {
        input_val iv = input_str(cmd);
        if (iv == eof) {
            printf("Server finishing it's work...\n");
            break;
        } else if (iv == bad) {
            printf("Unknown command, try help.\n");
            continue;
        }
        if (strcmp(cmd, "create") == 0) {
            iv = input_int(&pid);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            if (!create_node(pid)) {
                printf("Unable to create node with pid %d\n", pid);
            }
        } else if (strcmp(cmd, "remove") == 0) {
            iv = input_int(&pid);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            if (!remove_node(pid)) {
                printf("Unable to remove node with pid %d\n", pid);
            }
        } else if (strcmp(cmd, "exec") == 0) {
            send_exec(NULL, NULL);
        } else if (strcmp(cmd, "help") == 0) {
            help();
        } else if (strcmp(cmd, "print") == 0) {
            print_tree(AVL_TREE_PTR);
        } else {
            printf("Unknown command, try help.\n");
        }
    }
    terminate_all_nodes();
}

int 
main() {
    init_avl(&AVL_TREE_PTR);
    init_table();
    if (init_master_socket(&CONTEXT, &EXEC_PUB, &HEARTBIT_SUB) != 0) {
        return -1;
    }
    help();
    server_loop();
    deinit_avl(AVL_TREE_PTR);
    if (deinit_master_socket(CONTEXT, EXEC_PUB, HEARTBIT_SUB) != 0) {
        return -2;
    }
    return 0;
}
