#ifndef NETWORKING_H
#define NETWORKING_H
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>

#include "common.h"

#define PORT 7777
#define BACKLOG 4024

StatusCode create_server_socket(int *serverSocket);

StatusCode bind_listen_server_socket(int *serverSocket);

StatusCode accept_clients(int *serverSocket, int *clientSocket);

StatusCode create_conn_client_socket(int *client_socket, const char *ip, const int port);

StatusCode send_data(int socket, const void *data, size_t size);

StatusCode receive_data(int socket, void *data, size_t size);

#endif
