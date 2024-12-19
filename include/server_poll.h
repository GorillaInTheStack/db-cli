#ifndef SERVER_POOL_H
#define SERVER_POOL_H
#include <poll.h>
#include <string.h>

#include "protocol.h"
#include "context.h"

#define MAX_CLIENTS 256
#define BUFF_SIZE 4096

typedef struct
{
    int fd;
    State state;
    char buffer[4096];
} ClientState;

void init_clients(ClientState *states);
int find_free_slot(ClientState *states);
int find_slot_by_fd(ClientState *states, int fd);
StatusCode poll_loop(DBContext *ctx);
void handle_client_fsm(DBContext *ctx, ClientState *client);

#endif
