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

static const char* CLIENT_NAME = "./client";

static const int32_t MAX_PID_NUM = 64;
static const int32_t VOID_PID = INT_MAX;
static int32_t REAL_PID_TABLE[MAX_PID_NUM];

typedef enum input_val input_val;
enum input_val {
    ok, eof, bad
};

void 
skip_str() {
    int c = getchar();
    while (c != '\n' && c != EOF) {
        c = getchar();
    }
}

void
init_table() {
    for (int32_t i = 0; i < MAX_PID_NUM; ++i) {
        REAL_PID_TABLE[i] = VOID_PID;
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
printf("|     help     |               print usage                 |\n");
printf("|              |       (pids range from 0 to 63)           |\n");
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
    REAL_PID_TABLE[pid] = -1;
    //...
    return true;
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
        printf("client %s\n", client_id);
        execl(CLIENT_NAME, client_id, client_id, NULL);
    } else {
        REAL_PID_TABLE[pid] = fv;
    }
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
            printf("exec\n");
        } else if (strcmp(cmd, "help") == 0) {
            help();
        } else {
            printf("Unknown command, try help.\n");
        }
    }
}

int 
main() {
    init_table();
    server_loop();
    return 0;
}
