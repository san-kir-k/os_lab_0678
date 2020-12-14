#pragma once
#include <zmq.h>
#include <zmq_utils.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"

void        print_err_cmp(int32_t pid);
void        print_err_mas();

int32_t     init_master_socket(void** context, void** pub, void** sub);
int32_t     init_computing_socket(  void** context, void** left_pub, void** right_pub, void** parent_sub,
                                    void** hrbt_pub, char* socket_name, char* parent_name, int32_t pid);  
                                          
int32_t     deinit_master_socket(void* context, void* pub, void* sub);
int32_t     deinit_computing_socket(void* context, void* left_pub, void* right_pub,
                                    void* parent_sub, void* hrbt_pub, int32_t pid);                           

