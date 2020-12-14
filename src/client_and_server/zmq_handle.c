#include "zmq_handle.h"

static const int32_t TIME_MS = 100;

void
print_err_cmp(int32_t pid) {
    char msg[20];
    sprintf(msg, "ERROR, PID %d: ", pid);
    perror(msg);    
}

void
print_err_mas() {
    perror("ERROR, MASTER: ");   
}

int32_t
init_master_socket(void** context, void** pub, void** sub) {
    *context = zmq_ctx_new();
    if (*context == NULL) {
        fprintf(stderr, "MASTER: Unable to create context.\n");
        return -3;
    }

    void* publisher = zmq_socket(*context, ZMQ_PUB);
    if (publisher == NULL) {
        print_err_mas();
        return -4;
    }
    int32_t pc = zmq_bind(publisher, MASTER_SOCKET); 
    if (pc != 0) {
        print_err_mas();
        return -1;
    }
    *pub = publisher;

    void* subscriber = zmq_socket(*context, ZMQ_SUB);
    if (subscriber == NULL) {
        print_err_mas();
        return -5;
    }
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);
    zmq_setsockopt(subscriber, ZMQ_CONNECT_TIMEOUT, &TIME_MS, sizeof(TIME_MS));
    pc = zmq_connect(subscriber, MASTER_SOCKET); 
    if (pc != 0) {
        print_err_mas();
        return -2;
    }
    *sub = subscriber;
    return 0;
}

int32_t
init_computing_socket(  void** context, void** left_pub, void** right_pub, void** parent_sub,
                        void** hrbt_pub, char* socket_name, char* parent_name, int32_t pid) {
    *context = zmq_ctx_new();
    if (*context == NULL) {
        fprintf(stderr, "PID %d: Unable to create context.\n", pid);
        return -5;
    }

    void* left_publisher = zmq_socket(*context, ZMQ_PUB);
    if (left_publisher == NULL) {
        print_err_cmp(pid);
        return -6;
    }
    int32_t pc = zmq_bind(left_publisher, socket_name); 
    if (pc != 0) {
        print_err_cmp(pid);
        return -1;
    }
    *left_pub = left_publisher;

    void* right_publisher = zmq_socket(*context, ZMQ_PUB);
    if (right_publisher == NULL) {
        print_err_cmp(pid);
        return -7;
    }
    pc = zmq_bind(right_publisher, socket_name); 
    if (pc != 0) {
        print_err_cmp(pid);
        return -2;
    }
    *right_pub = right_publisher;

    void* hrbt_publisher = zmq_socket(*context, ZMQ_PUB);
    if (hrbt_publisher == NULL) {
        print_err_cmp(pid);
        return -8;
    }
    pc = zmq_bind(hrbt_publisher, socket_name); 
    if (pc != 0) {
        print_err_cmp(pid);
        return -3;
    }
    *hrbt_pub = hrbt_publisher;

    void* parent_subscriber = zmq_socket(*context, ZMQ_SUB);
    if (parent_subscriber == NULL) {
        print_err_cmp(pid);
        return -9;
    }
    zmq_setsockopt(parent_subscriber, ZMQ_SUBSCRIBE, NULL, 0);
    zmq_setsockopt(parent_subscriber, ZMQ_CONNECT_TIMEOUT, &TIME_MS, sizeof(TIME_MS));
    pc = zmq_connect(parent_subscriber, parent_name); 
    if (pc != 0) {
        print_err_cmp(pid);
        return -4;
    }
    *parent_sub = parent_subscriber;
    return 0;
}

int32_t
deinit_master_socket(void* context, void* pub, void* sub) {
    if (zmq_close(pub) != 0) {
        print_err_mas();
        return -1;
    }
    if (zmq_close(sub) != 0) {
        print_err_mas();
        return -2;
    }
    if (zmq_ctx_term(context) != 0) {
        print_err_mas();
        return -3;        
    }
    return 0;
}

int32_t
deinit_computing_socket(void* context, void* left_pub, void* right_pub,
                        void* parent_sub, void* hrbt_pub, int32_t pid) {
    if (zmq_close(left_pub) != 0) {
        print_err_cmp(pid);
        return -1;
    }
    if (zmq_close(right_pub) != 0) {
        print_err_cmp(pid);
        return -2;
    }
    if (zmq_close(hrbt_pub) != 0) {
        print_err_cmp(pid);
        return -3;
    }
    if (zmq_close(parent_sub) != 0) {
        print_err_cmp(pid);
        return -4;
    }
    if (zmq_ctx_term(context) != 0) {
        print_err_cmp(pid);
        return -5;        
    }
    return 0;
} 
