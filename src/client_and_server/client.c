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
static char     SOCKET_NAME[64];
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

void
init_cmp_name(int pid) {
    char pid_str[10];
    sprintf(pid_str, "%d", CLIENT_PID);
    strcpy(SOCKET_NAME, CMP_SOCKET_PATTERN);
    strcat(SOCKET_NAME, pid_str);
}

int32_t
compute(char* text, char* pattern, int32_t text_size, int32_t pattern_size, int32_t* res) {
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
computing_loop() {
    // while (true) {
        zmq_msg_t message;
        zmq_msg_init(&message);
        zmq_msg_recv(&message, PARENT_SOCKET, 0);
        zmq_msg_close(&message);
        printf("done\n");
        if (!zmq_msg_more (&message)) {
            printf("end\n");
            // break;      //  Last message frame
        }
    // }
}

int
main(int argc, char* argv[]) {
    if (signal(SIGTERM, process_sigterm) == SIG_ERR) {
        printf("err\n");
        return 1;
    }
    printf("In child with pid %d\n", atoi(argv[1]));
    CLIENT_PID = atoi(argv[1]);
    strcpy(PARENT_NAME,argv[2]);
    init_cmp_name(CLIENT_PID);
    if (init_computing_socket(  &CONTEXT, &LEFT_SOCKET, &RIGHT_SOCKET, &PARENT_SOCKET,
                                &HRBT_SOCKET, SOCKET_NAME, PARENT_NAME, CLIENT_PID) != 0) {
        return -1;
    }
    printf("p: %s, c: %s\n", PARENT_NAME, SOCKET_NAME);
    // computing_loop();
    if (deinit_computing_socket(CONTEXT, LEFT_SOCKET, RIGHT_SOCKET,
                                PARENT_SOCKET, HRBT_SOCKET, CLIENT_PID) != 0) {
        return -2;
    }
    return 0;
}
