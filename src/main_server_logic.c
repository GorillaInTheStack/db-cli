#include <unistd.h>
#include <string.h>

#include "networking.h"

StatusCode start_server(int argc, char *argv[])
{
    int serverSocket = -1;

    StatusCode status = create_server_socket(&serverSocket);
    if (status != STATUS_OK) {
        printf("Failed to create server socket\n");
        return status;
    }

    printf("Server socket created successfully: %d\n", serverSocket);

    status = bind_listen_server_socket(&serverSocket);
    if (status != STATUS_OK) {
        printf("Failed to bind or listen on server socket\n");
        if (status == STATUS_ERROR) close(serverSocket);
        return status;
    }

    while (1)
    {
        int clientSocket = -1;
        status = accept_clients(&serverSocket, &clientSocket);
        if (status != STATUS_OK) {
            printf("Failed to accept a client\n");
            if (status == STATUS_ERROR) close(serverSocket);
            return status;
        }
        char *str = "Hello World\n";

        write(clientSocket, str, strlen(str));
        close(clientSocket);
    }

    close(serverSocket);

    return STATUS_OK;

}
