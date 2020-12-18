#pragma once
#include <stdint.h>

#define MASTER_SOCKET_PUB "ipc://sockets/lab0678_master_p"
#define MASTER_SOCKET_SUB "ipc://sockets/lab0678_master_s"

#define CMP_SOCKET_PATTERN_L "ipc://sockets/lab0678_cmp_l_"
#define CMP_SOCKET_PATTERN_R "ipc://sockets/lab0678_cmp_r_"

#define CLIENT_PROG_NAME "./client"

#define MAX_PID_NUM 64
#define VOID_PID INT_MAX
#define MAX_STRLEN 64

#define OK 0

#define MASTER_CTX_CREATE_ERR -3
#define MASTER_PUB_CREATE_ERR -4
#define MASTER_PUB_BIND_ERR -1
#define MASTER_SUB_CREATE_ERR -5
#define MASTER_SUB_BIND_ERR -2

#define MASTER_CLOSE_PUB_ERR -1
#define MASTER_CLOSE_SUB_ERR -2
#define MASTER_CLOSE_CTX_ERR -3

#define CLIENT_CTX_CREATE_ERR -5
#define CLIENT_PUB_L_CREATE_ERR -6
#define CLIENT_PUB_L_BIND_ERR -1
#define CLIENT_PUB_R_CREATE_ERR -7
#define CLIENT_PUB_R_BIND_ERR -2
#define CLIENT_PUB_H_CREATE_ERR -8
#define CLIENT_PUB_H_CON_ERR -3
#define CLIENT_SUB_P_CREATE_ERR -9
#define CLIENT_SUB_P_CON_ERR -4

#define CLIENT_CLOSE_PUB_L_ERR -1
#define CLIENT_CLOSE_PUB_R_ERR -2
#define CLIENT_CLOSE_PUB_H_ERR -3
#define CLIENT_CLOSE_SUB_P_ERR -4
#define CLIENT_CLOSE_CTX_ERR -5

#define SERV_INIT_ERR -1
#define SERV_DEINIT_ERR -2
#define SERV_SIG_ERR -3

#define CLIENT_INIT_ERR -1
#define CLIENT_DEINIT_ERR -2
#define CLIENT_SIG_ERR -3
