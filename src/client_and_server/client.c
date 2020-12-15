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

#include "zmq_handle.h"
#include "defines.h"

int32_t         CLIENT_PID;
int32_t         PARENT_PID;
int32_t         TMP_PARENT_PID;
static char     SOCKET_NAME_L[MAX_STRLEN];
static char     SOCKET_NAME_R[MAX_STRLEN];
static char     SOCKET_NAME_H[MAX_STRLEN];
static char     PARENT_NAME[MAX_STRLEN];
static void*    CONTEXT;
static void*    LEFT_SOCKET;
static void*    RIGHT_SOCKET;  
static void*    PARENT_SOCKET;  
static void*    HRBT_SOCKET;  

void
process_sigterm(int32_t sig) {
    printf("Computing node %d finishing it's work...\n", CLIENT_PID);
    deinit_computing_socket(CONTEXT, LEFT_SOCKET, RIGHT_SOCKET,
                            PARENT_SOCKET, HRBT_SOCKET, CLIENT_PID);
    exit(EXIT_SUCCESS);
}

void 
mm_pass_rebind(int id, event* cmd) {
    if (CLIENT_PID == id) {
        TMP_PARENT_PID = cmd->change_to;
    } else {
        event sent_cmd;
        sent_cmd.cmd = rebind_cmd;
        sent_cmd.to = id;
        sent_cmd.change_to = cmd->change_to;
        zmq_msg_t zmqmsg;
        zmq_msg_init_size(&zmqmsg, sizeof(event));
        memcpy(zmq_msg_data(&zmqmsg), &sent_cmd, sizeof(event));
        if (id < CLIENT_PID) {
            zmq_msg_send(&zmqmsg, LEFT_SOCKET, 0);
        } else {
            zmq_msg_send(&zmqmsg, RIGHT_SOCKET, 0);
        }
        zmq_msg_close(&zmqmsg);
    }
}

void 
mm_pass_relax() {
    event sent_cmd;
    sent_cmd.cmd = relax_cmd;
    zmq_msg_t zmqmsg;
    zmq_msg_init_size(&zmqmsg, sizeof(event));
    memcpy(zmq_msg_data(&zmqmsg), &sent_cmd, sizeof(event));
    zmq_msg_send(&zmqmsg, LEFT_SOCKET, 0);
    zmq_msg_close(&zmqmsg);

    sent_cmd.cmd = relax_cmd;
    zmq_msg_init_size(&zmqmsg, sizeof(event));
    memcpy(zmq_msg_data(&zmqmsg), &sent_cmd, sizeof(event));
    zmq_msg_send(&zmqmsg, RIGHT_SOCKET, 0);
    zmq_msg_close(&zmqmsg);

    if (PARENT_PID != TMP_PARENT_PID) {
        // printf("NODE %d: Changing from %d to %d\n", CLIENT_PID, PARENT_PID, TMP_PARENT_PID);
        zmq_disconnect(PARENT_SOCKET, PARENT_NAME);
        if (CLIENT_PID < TMP_PARENT_PID) {
            if (TMP_PARENT_PID == -1) {
                sprintf(PARENT_NAME, MASTER_SOCKET_PUB);
            } else {
                sprintf(PARENT_NAME, CMP_SOCKET_PATTERN_L"%d", TMP_PARENT_PID);
            }
        } else {
            if (TMP_PARENT_PID == -1) {
                sprintf(PARENT_NAME, MASTER_SOCKET_PUB);
            } else {
                sprintf(PARENT_NAME, CMP_SOCKET_PATTERN_R"%d", TMP_PARENT_PID);
            }
        }
        // printf("parent %s\n", PARENT_NAME);
        zmq_connect(PARENT_SOCKET, PARENT_NAME); 
        PARENT_PID = TMP_PARENT_PID;
    }
}

int32_t
naive(char* text, char* pattern, int32_t text_size, int32_t pattern_size, int32_t* res) {
    int32_t i = 0;
    int32_t j = 0;
    int32_t matching_c = 0;
    while (i < text_size) {
        int32_t h = i;
        while (h < text_size && j < pattern_size && text[h] == pattern[j]) {
            h++;
            j++;
        }
        if (j == pattern_size) {
            res[matching_c] = i;
            matching_c++;
        }
        j = 0;
        i++;
    }
    return matching_c;
}

void
compute(event* e) {
    int32_t res[MAX_STRLEN];
    int32_t len = naive(e->text, e->pattern, e->text_size, e->pattern_size, res);
    printf("Result is:\n");
    if (len == 0) {
        printf("There are no occurrences.\n");
    } else {
        for (int32_t i = 0; i < len; ++i) {
            printf("%d;", res[i]);
        }
        printf("\n");
    }
}

void
computing_loop() {
    while (true) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        zmq_msg_init_size(&message, sizeof(event));
        zmq_msg_recv(&message, PARENT_SOCKET, 0);
        event e;
        memcpy(&e, zmq_msg_data(&message), sizeof(event));
        zmq_msg_close(&message);
        if (e.cmd == exec_cmd) {
            // printf("in node %d\n", CLIENT_PID);
            if (e.to != CLIENT_PID) {
                if (e.to > CLIENT_PID) {
                    send_to(RIGHT_SOCKET, &e);
                } else {
                    send_to(LEFT_SOCKET, &e);
                }
            } else {
                // printf("I am %d, my pid is %d\n", CLIENT_PID, getpid());
                // printf("P: %s\n", PARENT_NAME);
                compute(&e);
            }
        } else if (e.cmd == hrbt_cmd) {
            send_to(LEFT_SOCKET, &e);
            send_to(RIGHT_SOCKET, &e);
            for (int32_t i = 0; i < 5; ++i) {
                event e_copy = e;
                e_copy.to = CLIENT_PID;
                send_to(HRBT_SOCKET, &e_copy); 
                usleep(1e3 * e_copy.sleep_time);
            }          
        } else if (e.cmd == rebind_cmd) {
            mm_pass_rebind(e.to, &e);
        } else {
            mm_pass_relax();
        }
    }
}

int
main(int argc, char* argv[]) {
    if (signal(SIGTERM, process_sigterm) == SIG_ERR) {
        perror("ERROR: ");
        return CLIENT_SIG_ERR;
    }
    CLIENT_PID = atoi(argv[1]);
    strcpy(PARENT_NAME,argv[2]);
    PARENT_PID = atoi(argv[3]);
    TMP_PARENT_PID = atoi(argv[3]);
    init_cmp_name(CLIENT_PID, SOCKET_NAME_L, left);
    init_cmp_name(CLIENT_PID, SOCKET_NAME_R, right);
    strcpy(SOCKET_NAME_H, MASTER_SOCKET_SUB);
    if (init_computing_socket(  &CONTEXT, &LEFT_SOCKET, &RIGHT_SOCKET, &PARENT_SOCKET,
                                &HRBT_SOCKET, SOCKET_NAME_L, SOCKET_NAME_R, SOCKET_NAME_H,
                                PARENT_NAME, CLIENT_PID) != 0) {
        return CLIENT_INIT_ERR;
    }
    computing_loop();
    if (deinit_computing_socket(CONTEXT, LEFT_SOCKET, RIGHT_SOCKET,
                                PARENT_SOCKET, HRBT_SOCKET, CLIENT_PID) != 0) {
        return CLIENT_DEINIT_ERR;
    }
    return 0;
}
