#include "zmq_handle.h"

static const int32_t    TIME_MS = 100;
void*                   EXEC_PUB;
void*                   HEARTBIT_SUB;

void
create_message(zmq_msg_t* msg, event* e) {
    zmq_msg_init_size(msg, sizeof(event));
    memcpy(zmq_msg_data(msg), e, sizeof(event));
}

void
init_cmp_name(int pid, char* name, socket_type node_type) {
    char pid_str[MAX_STRLEN];
    sprintf(pid_str, "%d", pid);
    if (node_type == left) {
        strcpy(name, CMP_SOCKET_PATTERN_L);
    } else if (node_type == right) {
        strcpy(name, CMP_SOCKET_PATTERN_R);
    }
    strcat(name, pid_str);
}

void        
send_to(void* socket, event* e) {
    zmq_msg_t message;
    zmq_msg_init(&message);
    create_message(&message, e);
    zmq_msg_send(&message, socket, 0);
    zmq_msg_close(&message);
}

void
mm_send_rebind(int id, int target_id) {
    event sent_cmd;
    sent_cmd.to = id;
    sent_cmd.cmd = rebind_cmd;
    sent_cmd.change_to = target_id;
    zmq_msg_t zmqmsg;
    zmq_msg_init_size(&zmqmsg, sizeof(event));
    memcpy(zmq_msg_data(&zmqmsg), &sent_cmd, sizeof(event));
    int send = zmq_msg_send(&zmqmsg, EXEC_PUB, 0);
    zmq_msg_close(&zmqmsg);
}

void 
mm_send_relax() {
    event sent_cmd;
    sent_cmd.cmd = relax_cmd;
    zmq_msg_t zmqmsg;
    zmq_msg_init_size(&zmqmsg, sizeof(event));
    memcpy(zmq_msg_data(&zmqmsg), &sent_cmd, sizeof(event));
    int send = zmq_msg_send(&zmqmsg, EXEC_PUB, 0);
    zmq_msg_close(&zmqmsg);
}

void
print_err_cmp(int32_t pid) {
    char msg[MAX_STRLEN];
    sprintf(msg, "ERROR, PID %d", pid);
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
        return MASTER_CTX_CREATE_ERR;
    }

    void* publisher = zmq_socket(*context, ZMQ_PUB);
    if (publisher == NULL) {
        print_err_mas();
        return MASTER_PUB_CREATE_ERR;
    }
    int32_t pc = zmq_bind(publisher, MASTER_SOCKET_PUB); 
    if (pc != 0) {
        print_err_mas();
        return MASTER_PUB_BIND_ERR;
    }
    *pub = publisher;

    void* subscriber = zmq_socket(*context, ZMQ_SUB);
    if (subscriber == NULL) {
        print_err_mas();
        return MASTER_SUB_CREATE_ERR;
    }
    zmq_setsockopt(subscriber, ZMQ_SUBSCRIBE, NULL, 0);
    zmq_setsockopt(subscriber, ZMQ_CONNECT_TIMEOUT, &TIME_MS, sizeof(TIME_MS));
    pc = zmq_bind(subscriber, MASTER_SOCKET_SUB); 
    if (pc != 0) {
        print_err_mas();
        return MASTER_SUB_BIND_ERR;
    }
    *sub = subscriber;
    return OK;
}

int32_t
init_computing_socket(  void** context, void** left_pub, void** right_pub, void** parent_sub,
                        void** hrbt_pub, char* socket_name_l, char* socket_name_r,
                        char* socket_name_h, char* parent_name, int32_t pid) {
    *context = zmq_ctx_new();
    if (*context == NULL) {
        fprintf(stderr, "PID %d: Unable to create context.\n", pid);
        return CLIENT_CTX_CREATE_ERR;
    }

    void* left_publisher = zmq_socket(*context, ZMQ_PUB);
    if (left_publisher == NULL) {
        print_err_cmp(pid);
        return CLIENT_PUB_L_CREATE_ERR;
    }
    int32_t pc = zmq_bind(left_publisher, socket_name_l); 
    if (pc != 0) {
        print_err_cmp(pid);
        return CLIENT_PUB_L_BIND_ERR;
    }
    *left_pub = left_publisher;

    void* right_publisher = zmq_socket(*context, ZMQ_PUB);
    if (right_publisher == NULL) {
        print_err_cmp(pid);
        return CLIENT_PUB_R_CREATE_ERR;
    }
    pc = zmq_bind(right_publisher, socket_name_r); 
    if (pc != 0) {
        print_err_cmp(pid);
        return CLIENT_PUB_R_BIND_ERR;
    }
    *right_pub = right_publisher;

    void* hrbt_publisher = zmq_socket(*context, ZMQ_PUB);
    if (hrbt_publisher == NULL) {
        print_err_cmp(pid);
        return CLIENT_PUB_H_CREATE_ERR;
    }
    pc = zmq_connect(hrbt_publisher, socket_name_h); 
    if (pc != 0) {
        print_err_cmp(pid);
        return CLIENT_PUB_H_CON_ERR;
    }
    *hrbt_pub = hrbt_publisher;

    void* parent_subscriber = zmq_socket(*context, ZMQ_SUB);
    if (parent_subscriber == NULL) {
        print_err_cmp(pid);
        return CLIENT_SUB_P_CREATE_ERR;
    }
    zmq_setsockopt(parent_subscriber, ZMQ_SUBSCRIBE, NULL, 0);
    zmq_setsockopt(parent_subscriber, ZMQ_CONNECT_TIMEOUT, &TIME_MS, sizeof(TIME_MS));
    pc = zmq_connect(parent_subscriber, parent_name); 
    if (pc != 0) {
        print_err_cmp(pid);
        return CLIENT_SUB_P_CON_ERR;
    }
    *parent_sub = parent_subscriber;
    return OK;
}

int32_t
deinit_master_socket(void* context, void* pub, void* sub) {
    if (zmq_close(pub) != 0) {
        print_err_mas();
        return MASTER_CLOSE_PUB_ERR;
    }
    if (zmq_close(sub) != 0) {
        print_err_mas();
        return MASTER_CLOSE_SUB_ERR;
    }
    if (zmq_ctx_term(context) != 0) {
        print_err_mas();
        return MASTER_CLOSE_CTX_ERR;        
    }
    return OK;
}

int32_t
deinit_computing_socket(void* context, void* left_pub, void* right_pub,
                        void* parent_sub, void* hrbt_pub, int32_t pid) {
    if (zmq_close(left_pub) != 0) {
        print_err_cmp(pid);
        return CLIENT_CLOSE_PUB_L_ERR;
    }
    if (zmq_close(right_pub) != 0) {
        print_err_cmp(pid);
        return CLIENT_CLOSE_PUB_R_ERR;
    }
    if (zmq_close(hrbt_pub) != 0) {
        print_err_cmp(pid);
        return CLIENT_CLOSE_PUB_H_ERR;
    }
    if (zmq_close(parent_sub) != 0) {
        print_err_cmp(pid);
        return CLIENT_CLOSE_SUB_P_ERR;
    }
    if (zmq_ctx_term(context) != 0) {
        print_err_cmp(pid);
        return CLIENT_CLOSE_CTX_ERR;        
    }
    return OK;
} 
