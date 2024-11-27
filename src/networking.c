
#include "networking.h"

StatusCode create_server_socket(int *serverSocket)
{
    if (!serverSocket){
        printf("Error: serverSocket pointer is null\n");
        return STATUS_INVALID_ARGUMENT;
    }

    int tmpServerSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (tmpServerSocket == -1) {
        perror("socket");
        close(tmpServerSocket);
        return STATUS_ERROR;
    }

    int opt = 1;
    int ret = setsockopt(tmpServerSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(int));

    if (ret == -1) {
        perror("setsockopt");
        close(tmpServerSocket);
        return STATUS_ERROR;
    }

    *serverSocket = tmpServerSocket;
    return STATUS_OK;
}

StatusCode bind_listen_server_socket(const int *serverSocket)
{
    if (!serverSocket){
        printf("Error: serverSocket pointer is null\n");
        return STATUS_INVALID_ARGUMENT;
    }

    struct sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = convert_ushort(PORT, true);

    int ret = bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress));

    if (ret == -1) {
        perror("bind");
        return STATUS_ERROR;
    }

    ret = listen(serverSocket, BACKLOG);

    if (ret == -1) {
        perror("listen");
        return STATUS_ERROR;
    }

    return STATUS_OK;
}

StatusCode accept_clients(const int *serverSocket, int *clientSocket)
{
    if (!serverSocket || !clientSocket){
        printf("Error: accept_clients got invalid args\n");
        return STATUS_INVALID_ARGUMENT;
    }

    struct sockaddr_in tmpClientAddress = {0};
    socklen_t clientAddrLen = sizeof(tmpClientAddress);
    int tmpClientSocket = accept(serverSocket, (struct sockaddr *)&tmpClientAddress, &clientAddrLen);

    if (tmpClientSocket == -1) {
        perror("accept");
        close(tmpClientSocket);
        return STATUS_ERROR;
    }

    *clientSocket = tmpClientSocket;
    printf("Accepted connection from client %s:%s", inet_ntoa(tmpClientAddress.sin_addr), convert_ushort(tmpClientAddress.sin_port, false));
    return STATUS_OK;
}

StatusCode create_conn_client_socket(int *client_socket, const char *ip, const int port)
{
    if (!client_socket || !ip || port <= 0){
        printf("Error: create_client_socket got invalid args\n");
        return STATUS_INVALID_ARGUMENT;
    }

    struct sockaddr_in serverInfo = {0};
    serverInfo.sin_family = AF_INET;
    serverInfo.sin_addr.s_addr = inet_addr(ip);
    serverInfo.sin_port = convert_ushort(port, true);

    int tmpClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    int ret = connect(tmpClientSocket, (struct sockaddr *)&serverInfo, sizeof(serverInfo));

    if (ret == -1) {
        perror("connect");
        close(tmpClientSocket);
        return STATUS_ERROR;
    }

    if (tmpClientSocket == -1) {
        perror("socket");
        close(tmpClientSocket);
        return STATUS_ERROR;
    }

    *client_socket = tmpClientSocket;
    return STATUS_OK;

}

StatusCode send_data(int socket, const void *data, size_t size)
{
    return STATUS_OK;
}

StatusCode receive_data(int socket, void *data, size_t size)
{
    return STATUS_OK;
}
