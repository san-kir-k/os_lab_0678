#pragma once
#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "defines.h"

typedef enum {
    exec_cmd,
    hrbt_cmd
} cmd_type;

typedef struct {
    char text[MAX_STRLEN];
    char pattern[MAX_STRLEN];
    int32_t text_size;
    int32_t pattern_size;
    int32_t to;
    cmd_type cmd;
    int32_t sleep_time;
} event;

typedef enum {
    left,
    right,
    hrbt
} socket_type;

void        create_message(zmq_msg_t* msg, event* e);
void        init_cmp_name(int pid, char* name, socket_type node_type);

void        send_to(void* socket, event* e);

void        print_err_cmp(int32_t pid);
void        print_err_mas();

int32_t     init_master_socket(void** context, void** pub, void** sub);
int32_t     init_computing_socket(  void** context, void** left_pub, void** right_pub, void** parent_sub,
                                    void** hrbt_pub, char* socket_name_l, char* socket_name_r,
                                    char* socket_name_h, char* parent_name, int32_t pid);  
                                          
int32_t     deinit_master_socket(void* context, void* pub, void* sub);
int32_t     deinit_computing_socket(void* context, void* left_pub, void* right_pub,
                                    void* parent_sub, void* hrbt_pub, int32_t pid);                           

