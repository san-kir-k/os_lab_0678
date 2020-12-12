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

static int32_t CLIENT_PID = INT_MAX;

void
compute() {
    printf("Doing some work.\n");
    sleep(1);
}

void
process_sigterm(int32_t sig) {
    printf("Computing node %d finishing it's work...\n", CLIENT_PID);
    exit(EXIT_SUCCESS);
}

int
main(int argc, char* argv[]) {
    printf("In child with pid %d\n", atoi(argv[1]));
    CLIENT_PID = atoi(argv[1]);
    int count = 0;
    while (count < 60) {
        if (signal(SIGTERM, process_sigterm) == SIG_ERR) {
            printf("err\n");
            return 1;
        }
        sleep(1);
        count++;
    }
    return 0;
}
