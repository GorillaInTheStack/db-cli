#ifndef PROTOCOL_H
#define PROTOCOL_H
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"

#define PROTO_VER 1

typedef enum
{
    STATE_NEW,
    STATE_CONNECTED,
    STATE_DISCONNECTED,
    STATE_HELLO,
    STATE_MSG,
    STATE_GOODBYE,
} State;

typedef enum
{
    MSG_HELLO_REQ,
    MSG_HELLO_RESP,
    MSG_EMPLOYEE_LIST_REQ,
    MSG_EMPLOYEE_LIST_RESP,
    MSG_EMPLOYEE_ADD_REQ,
    MSG_EMPLOYEE_ADD_RESP,
    MSG_EMPLOYEE_DEL_REQ,
    MSG_EMPLOYEE_DEL_RESP,
    MSG_EMPLOYEE_UPDATE_REQ,
    MSG_EMPLOYEE_UPDATE_RESP,
    MSG_ERROR,
} DBProtoType;

typedef struct
{
    uint16_t proto;
} DBProtoHelloReq;

typedef struct
{
    uint16_t proto;
} DBProtoHelloResp;

typedef struct
{
    uint8_t data[1024];
} DBProtoEmployeeAddReq;

typedef struct
{
    uint8_t data[1024];
} DBProtoEmployeeAddResp;

typedef struct
{
    DBProtoType type;
    unsigned int len;
} DBProtoHeader;

StatusCode send_hello(int fd);
StatusCode send_employee(int fd, char *employee_str);
StatusCode send_list(int fd);
StatusCode send_update_hours(int fd, char *name, int hours);
StatusCode send_delete(int fd, char *name);

#endif
