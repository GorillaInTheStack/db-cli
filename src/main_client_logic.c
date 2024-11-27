#include <unistd.h>

#include "networking.h"

StatusCode start_client(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <server ip>\n");
        return STATUS_OK;
    }

    char buf[4096] = {0};

    int socket = -1;
    char *ip = argv[1];
    int port = PORT;
    StatusCode status = create_conn_client_socket(&socket, ip, port);

    read(socket, buf, sizeof(buf));

    printf("Received %s\n", buf);

    close(socket);

    return STATUS_OK;
}