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
extern void*        EXEC_PUB;
extern void*        HEARTBIT_SUB;

void
create_event(event* e, char* text, char* pattern, int32_t pid, cmd_type cmd, int32_t sleep_time) {
    strcpy(e->text, text);
    strcpy(e->pattern, pattern);
    e->text_size = 0;
    for (int32_t i = 0; text[i] != '\0'; i++) {
        e->text_size++;
    }
    e->pattern_size = 0;
    for (int32_t i = 0; pattern[i] != '\0'; i++) {
        e->pattern_size++;
    }
    e->to = pid;
    e->cmd = cmd;
    e->sleep_time = sleep_time;
}

void
init_table() {
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        REAL_PID_TABLE[i] = VOID_PID;
    }
}

bool
remove_node(int32_t pid) {
    if (pid < 0 || pid > MAX_PID_NUM - 1) {
        printf("Invalid pid.\n");
        return false;
    }
    int32_t rpid = REAL_PID_TABLE[pid];
    if (rpid == VOID_PID) {
        printf("There is no node with this pid.\n");
        return false;
    }
    REAL_PID_TABLE[pid] = VOID_PID;
    remove_from_tree(AVL_TREE_PTR, pid);
    sleep(1);
    kill(rpid, SIGTERM);
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

void
process_signal(int32_t sig) {
    terminate_all_nodes();
    deinit_avl(AVL_TREE_PTR);
    deinit_master_socket(CONTEXT, EXEC_PUB, HEARTBIT_SUB);
    exit(EXIT_SUCCESS);
}

bool
create_node(int32_t pid) {
    if (pid < 0 || pid > MAX_PID_NUM - 1) {
        printf("Invalid pid.\n");
        return false;
    }
    if (REAL_PID_TABLE[pid] != VOID_PID) {
        printf("Node with this pid has already created.\n");
        return false;
    }
    char parent_name[MAX_STRLEN];
    int32_t path_len = MAX_STRLEN;
    int32_t path[MAX_STRLEN];
    add_to_tree(AVL_TREE_PTR, pid);
    get_path(AVL_TREE_PTR, pid, &path_len, path);
    int32_t parent_pid = -1;
    if (path_len == 1) {
        strcpy(parent_name, MASTER_SOCKET_PUB);
    } else {
        parent_pid = get_parent_id(AVL_TREE_PTR, pid);
        if (pid < parent_pid) {
            init_cmp_name(parent_pid, parent_name, left);
        } else {
            init_cmp_name(parent_pid, parent_name, right);
        }
    }
    int32_t fv = fork();
    if (fv < 0) {
        remove_from_tree(AVL_TREE_PTR, pid);
        printf("Unable to create node, fork err.\n");
        return false;
    } else if (fv == 0) {
        char client_id[MAX_STRLEN];
        sprintf(client_id, "%d", pid);
        char client_name[MAX_STRLEN];
        sprintf(client_name, "client_%d", pid);
        char parent_id[MAX_STRLEN];
        sprintf(parent_id, "%d", parent_pid);
        execl(CLIENT_PROG_NAME, client_name, client_id, parent_name, parent_id, NULL);
    } else {
        REAL_PID_TABLE[pid] = fv;
    }
    return true;
}

bool
send_exec(char* text, char* pattern, int32_t pid) {
    if (pid < 0 || pid > MAX_PID_NUM - 1) {
        printf("Invalid pid.\n");
        return false;
    }
    if (REAL_PID_TABLE[pid] == VOID_PID) {
        printf("There is no node with this pid.\n");
        return false;
    }
    event e;
    create_event(&e, text, pattern, pid, exec_cmd, 0);
    send_to(EXEC_PUB, &e);
    return true;
}

void 
send_heartbit(int32_t input_time) {
    event e;
    create_event(&e, "", "", -1, hrbt_cmd, input_time);
    send_to(EXEC_PUB, &e);
}

void kill_child(int32_t pid) {
    int32_t rpid = REAL_PID_TABLE[pid];
    if (rpid == VOID_PID) {
        printf("There is no node with this pid.\n");
        return;
    }
    REAL_PID_TABLE[pid] = VOID_PID;
    kill(rpid, SIGTERM);
}

void
heartbit(int32_t input_time) { 
    int32_t time_to_wait = 4 * input_time;
    zmq_setsockopt(HEARTBIT_SUB, ZMQ_RCVTIMEO, &time_to_wait, sizeof(time_to_wait));
    int32_t heartbit_table[MAX_PID_NUM];
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        if (REAL_PID_TABLE[i] == VOID_PID) {
            heartbit_table[i] = -1;
        } else {
            heartbit_table[i] = 0;
        }
    }
    send_heartbit(input_time);
    while (true) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        zmq_msg_init_size(&message, sizeof(event));
        int32_t rv = zmq_msg_recv(&message, HEARTBIT_SUB, 0);
        if (rv == -1 && errno == EAGAIN) {
            break;
        }
        event* e = (event*)zmq_msg_data(&message);
        heartbit_table[e->to]++;
        zmq_msg_close(&message);        
    }
    int32_t count_of_dead = 0;
    int32_t count_of_void = 0;
    int32_t dead_table[MAX_PID_NUM];
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        if (heartbit_table[i] == 0) {
            printf("OK: %d\n", i);
            dead_table[count_of_dead] = i;
            count_of_dead++;
        } else if (heartbit_table[i] == -1) {
            count_of_void++;
        }
    }
    if (count_of_void == MAX_PID_NUM) {
        printf("There are no alive nodes now.\n");
    } else if (count_of_dead == 0) {
        printf("All is alive.\n");
    }
    if (count_of_dead != 0) {
        delete_subtree(AVL_TREE_PTR, dead_table, count_of_dead);
        sleep(1);
        for (int i = 0; i < count_of_dead; ++i) {
            kill_child(dead_table[i]);
        }
    }
}

void
print_table() {
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        if (REAL_PID_TABLE[i] != VOID_PID) {
            printf("Client id: %d, pid: %d\n", i, REAL_PID_TABLE[i]);
        }
    }
}

void 
server_loop() {
    char cmd[MAX_STRLEN];
    int32_t pid = VOID_PID;
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
            } else {
                printf("OK: %d\n", REAL_PID_TABLE[pid]);
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
            } else {
                printf("OK\n");
            }
        } else if (strcmp(cmd, "exec") == 0) {
            char text[MAX_STRLEN];
            char pattern[MAX_STRLEN];
            iv = input_int(&pid);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            iv = input_str(text);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            iv = input_str(pattern);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            send_exec(text, pattern, pid);
        } else if (strcmp(cmd, "help") == 0) {
            help();
        } else if (strcmp(cmd, "heartbit") == 0) {
            int32_t input_time;
            iv = input_int(&input_time);
            if (iv == eof) {
                printf("Server finishing it's work...\n");
                break;
            } else if (iv == bad) {
                printf("Unknown arg, try help.\n");
                continue;
            }
            heartbit(input_time);
        } else if (strcmp(cmd, "print") == 0) {
            print_tree(AVL_TREE_PTR);
        } else if (strcmp(cmd, "table") == 0) {
            print_table();
        } else {
            printf("Unknown command, try help.\n");
        }
    }
    terminate_all_nodes();
}

int 
main() {
    if (signal(SIGSEGV, process_signal) == SIG_ERR) {
        perror("ERROR: ");
        return SERV_SIG_ERR;
    }
    if (signal(SIGINT, process_signal) == SIG_ERR) {
        perror("ERROR: ");
        return SERV_SIG_ERR;
    }
    init_avl(&AVL_TREE_PTR);
    init_table();
    if (init_master_socket(&CONTEXT, &EXEC_PUB, &HEARTBIT_SUB) != 0) {
        return SERV_INIT_ERR;
    }
    help();
    server_loop();
    deinit_avl(AVL_TREE_PTR);
    if (deinit_master_socket(CONTEXT, EXEC_PUB, HEARTBIT_SUB) != 0) {
        return SERV_DEINIT_ERR;
    }
    return 0;
}
