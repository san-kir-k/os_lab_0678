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

static int32_t  CLIENT_PID;
static char     SOCKET_NAME_L[64];
static char     SOCKET_NAME_R[64];
static char     SOCKET_NAME_H[64];
static char     PARENT_NAME[64];
static void*    CONTEXT;
static void*    LEFT_SOCKET;
static void*    RIGHT_SOCKET;  
static void*    PARENT_SOCKET;  
static void*    HRBT_SOCKET;    

void
process_sigterm(int32_t sig) {
    printf("Computing node %d finishing it's work...\n", CLIENT_PID);
    exit(EXIT_SUCCESS);
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
    int32_t res[128];
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
        event* e = (event*)zmq_msg_data(&message);
        zmq_msg_close(&message);
        if (e->to != CLIENT_PID) {
            zmq_msg_t message_to_son;
            zmq_msg_init(&message_to_son);
            create_message(&message_to_son, e);
            if (e->to > CLIENT_PID) {
                zmq_msg_send(&message_to_son, RIGHT_SOCKET, 0);
            } else {
                zmq_msg_send(&message_to_son, LEFT_SOCKET, 0);
            }
            zmq_msg_close(&message_to_son);
        } else {
            compute(e);
        }
    }
}

int
main(int argc, char* argv[]) {
    if (signal(SIGTERM, process_sigterm) == SIG_ERR) {
        perror("ERROR: ");
        return 1;
    }
    CLIENT_PID = atoi(argv[1]);
    strcpy(PARENT_NAME,argv[2]);
    init_cmp_name(CLIENT_PID, SOCKET_NAME_L, left);
    init_cmp_name(CLIENT_PID, SOCKET_NAME_R, right);
    init_cmp_name(CLIENT_PID, SOCKET_NAME_H, hrbt);
    if (init_computing_socket(  &CONTEXT, &LEFT_SOCKET, &RIGHT_SOCKET, &PARENT_SOCKET,
                                &HRBT_SOCKET, SOCKET_NAME_L, SOCKET_NAME_R, SOCKET_NAME_H,
                                PARENT_NAME, CLIENT_PID) != 0) {
        return -1;
    }
    computing_loop();
    if (deinit_computing_socket(CONTEXT, LEFT_SOCKET, RIGHT_SOCKET,
                                PARENT_SOCKET, HRBT_SOCKET, CLIENT_PID) != 0) {
        return -2;
    }
    return 0;
}
