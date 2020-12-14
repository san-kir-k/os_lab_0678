#pragma once
#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

typedef struct {
    char text[128];
    char pattern[128];
    int32_t text_size;
    int32_t pattern_size;
    int32_t to;
} event;

typedef enum {
    left,
    right,
    hrbt
} type;

void        create_message(zmq_msg_t* msg, event* e);
void        init_cmp_name(int pid, char* name, type node_type);

void        print_err_cmp(int32_t pid);
void        print_err_mas();

int32_t     init_master_socket(void** context, void** pub, void** sub);
int32_t     init_computing_socket(  void** context, void** left_pub, void** right_pub, void** parent_sub,
                                    void** hrbt_pub, char* socket_name_l, char* socket_name_r,
                                    char* socket_name_h, char* parent_name, int32_t pid);  
                                          
int32_t     deinit_master_socket(void* context, void* pub, void* sub);
int32_t     deinit_computing_socket(void* context, void* left_pub, void* right_pub,
                                    void* parent_sub, void* hrbt_pub, int32_t pid);                           

