#include "server_poll.h"
#include "networking.h"
#include "context.h"
#include "file.h"
#include "parse.h"

ClientState clientStates[MAX_CLIENTS];

void init_clients(ClientState *states)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        states[i].fd = -1;
        states[i].state = STATE_NEW;
        memset(&states[i].buffer, '\0', BUFF_SIZE);
    }
}

int find_free_slot(ClientState *states)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (states[i].fd == -1)
        {
            return i;
        }
    }
    return STATUS_ERROR;
}

int find_slot_by_fd(ClientState *states, int fd)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (states[i].fd == fd)
        {
            return i;
        }
    }
    return STATUS_ERROR;
}

void fsm_reply_error(ClientState *client, DBProtoHeader *hdr)
{
    hdr->type = convert_uint(MSG_ERROR, true);
    hdr->len = convert_ushort(0, true);

    int bytes = write(client->fd, hdr, sizeof(DBProtoHeader));
    if (bytes < 0)
    {
        perror("write");
        printf("Error sending error msg to client\n");
    }
}

void fsm_reply_hello(ClientState *client, DBProtoHeader *hdr)
{
    hdr->type = convert_uint(MSG_HELLO_RESP, true);
    hdr->len = convert_ushort(1, true);
    DBProtoHelloResp *hello_resp = (DBProtoHelloResp *)&hdr[1];
    hello_resp->proto = convert_ushort(PROTO_VER, true);

    int bytes = write(client->fd, hdr, sizeof(DBProtoHeader) + sizeof(DBProtoHelloResp));
    if (bytes < 0)
    {
        perror("write");
        printf("Error sending error msg to client\n");
    }
}

void fsm_reply_success(ClientState *client, DBProtoHeader *hdr, DBProtoType resp_type)
{
    hdr->type = convert_uint(resp_type, true);
    hdr->len = convert_ushort(0, true);

    int bytes = write(client->fd, hdr, sizeof(DBProtoHeader));
    if (bytes < 0)
    {
        perror("write");
        printf("Error sending error msg to client\n");
    }
}

void fsm_reply_list_employees(DBContext *ctx, ClientState *client, DBProtoHeader *hdr)
{
    hdr->type = convert_uint(MSG_EMPLOYEE_LIST_RESP, true);
    hdr->len = convert_ushort(ctx->header->count, true);
    int bytes = write(client->fd, hdr, sizeof(DBProtoHeader));
    if (bytes < 0)
    {
        perror("write");
        printf("Error sending number of elements to client\n");
    }

    Employee *employee = (Employee *)&hdr[1];

    for (int i = 0; i < ctx->header->count; i++)
    {
        strncpy(employee->name, ctx->employees[i].name, sizeof(employee->name));
        strncpy(employee->address, ctx->employees[i].address, sizeof(employee->address));
        employee->hours = convert_uint(ctx->employees[i].hours, true);
        write(client->fd, employee, sizeof(Employee));
    }
}

void handle_client_fsm(DBContext *ctx, ClientState *client)
{
    DBProtoHeader *hdr = (DBProtoHeader *)client->buffer;

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_uint(hdr->len, false);

    if (client->state == STATE_HELLO)
    {
        if (hdr->type != MSG_HELLO_REQ || hdr->len != 1)
        {
            printf("Did not receive hello request body in client hello state\n");
            fsm_reply_error(client, hdr);
        }

        DBProtoHelloReq *hello = (DBProtoHelloReq *)&hdr[1];
        hello->proto = convert_ushort(hello->proto, false);
        if (hello->proto != PROTO_VER)
        {
            printf("Protocol mismatch with client -> client sent %d\n", hello->proto);
            fsm_reply_error(client, hdr);
        }

        printf("Client sent hello request with version %d\n", hello->proto);
        fsm_reply_hello(client, hdr);
        client->state = STATE_MSG;
    }

    if (client->state == STATE_MSG)
    {
        // add employee req
        if (hdr->type == MSG_EMPLOYEE_ADD_REQ)
        {
            DBProtoEmployeeAddReq *employee = (DBProtoEmployeeAddReq *)&hdr[1];

            StatusCode status = add_employee(ctx, (char *)employee->data);
            if (status == STATUS_OK)
            {
                status = output_file(ctx);
                if (status < 0)
                {
                    printf("Error saving add employee change to file\n");
                    fsm_reply_error(client, hdr);
                    return;
                }
                printf("Employee added.\n");

                fsm_reply_success(client, hdr, MSG_EMPLOYEE_ADD_RESP);
            }
            else
            {
                printf("Error, unable to add employee.\n");
                fsm_reply_error(client, hdr);
                return;
            }
        }

        // list employees req
        if (hdr->type == MSG_EMPLOYEE_LIST_REQ)
        {
            fsm_reply_list_employees(ctx, client, hdr);
        }

        // delete employee req
        if (hdr->type == MSG_EMPLOYEE_DEL_REQ)
        {
            char *name = (char *)&hdr[1];
            StatusCode status = del_employee_by_name(ctx, name);
            if (status == STATUS_OK)
            {
                status = output_file(ctx);
                if (status < 0)
                {
                    printf("Error saving delete employee change to file\n");
                    fsm_reply_error(client, hdr);
                    return;
                }
                printf("Employee deleted.\n");

                fsm_reply_success(client, hdr, MSG_EMPLOYEE_DEL_RESP);
            }
            else
            {
                printf("Error, unable to delete employee.\n");
                fsm_reply_error(client, hdr);
                return;
            }
        }

        // update employee hours req
        if (hdr->type == MSG_EMPLOYEE_UPDATE_REQ)
        {
            int *hours = (int *)&hdr[1];
            char *name = (char *)&hdr[2];

            StatusCode status = update_employee_hours_by_name(ctx, name, *hours);
            if (status == STATUS_OK)
            {
                status = output_file(ctx);
                if (status < 0)
                {
                    printf("Error saving update employee change to file\n");
                    fsm_reply_error(client, hdr);
                    return;
                }
                printf("Employee updated.\n");

                fsm_reply_success(client, hdr, MSG_EMPLOYEE_DEL_RESP);
            }
            else
            {
                printf("Error, unable to update employee.\n");
                fsm_reply_error(client, hdr);
                return;
            }
        }
    }
}

StatusCode poll_loop(DBContext *ctx)
{
    struct pollfd fds[MAX_CLIENTS + 1];
    int nfds = 1;
    int opt = 1;
    int freeSlot = -1;

    init_clients(clientStates);

    int serverSocket = -1;

    StatusCode status = create_server_socket(&serverSocket);
    if (status != STATUS_OK)
    {
        printf("Failed to create server socket\n");
        return status;
    }

    printf("Server socket created successfully: %d\n", serverSocket);

    status = bind_listen_server_socket(&serverSocket);
    if (status != STATUS_OK)
    {
        printf("Failed to bind or listen on server socket\n");
        if (status == STATUS_ERROR)
            close(serverSocket);
        return status;
    }

    memset(fds, 0, sizeof(fds));
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;
    nfds = 1;

    while (1)
    {

        int ii = 1;
        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (clientStates[i].fd != -1)
            {
                fds[ii].fd = clientStates[i].fd; // ii -> avoid serverSocket
                fds[ii].events = POLLIN;
                ii++;
            }
        }

        int n_events = poll(fds, nfds, -1);
        if (n_events == -1)
        {
            perror("poll");
            return STATUS_ERROR;
        }

        // Process server socket events first
        if (fds[0].revents & POLLIN)
        {
            int clientSocket = -1;
            status = accept_clients(&serverSocket, &clientSocket);
            if (status != STATUS_OK)
            {
                printf("Failed to accept a client\n");
                continue;
            }

            freeSlot = find_free_slot(clientStates);
            if (freeSlot == -1)
            {
                printf("Server full: closing new connection\n");
                close(clientSocket);
            }
            else
            {
                clientStates[freeSlot].fd = clientSocket;
                clientStates[freeSlot].state = STATE_HELLO;
                nfds++;
                printf("Slot %d has fd %d\n", freeSlot, clientStates[freeSlot].fd);
            }
            n_events--;
        }

        // process client sockets events
        for (int i = 1; ((i <= nfds) && (n_events > 0)); i++)
        {
            if (fds[i].revents & POLLIN)
            {
                n_events--;

                int fd = fds[i].fd;
                int slot = find_slot_by_fd(clientStates, fd);
                if (slot == -1)
                {
                    printf("Client went missing...\n");
                    close(fd);
                    continue;
                }

                ssize_t bytes_read = read(fd, &clientStates[slot].buffer, BUFF_SIZE);
                if (bytes_read <= 0)
                {
                    close(fd);
                    clientStates[slot].fd = -1;
                    clientStates[slot].state = STATE_DISCONNECTED;
                    memset(clientStates[slot].buffer, '\0', BUFF_SIZE);
                    printf("Client disconnected\n");
                    nfds--;
                }
                else
                {
                    // printf("Received data from client -> %s\n", clientStates[slot].buffer);
                    handle_client_fsm(ctx, &clientStates[slot]);
                }
            }
        }
        char *str = "Hello World\n";

        // write(clientSocket, str, strlen(str));
        // close(clientSocket);
    }
    close(serverSocket);
}
