#include <unistd.h>
#include <getopt.h>
#include <stdlib.h>
#include <string.h>

#include "networking.h"
#include "protocol.h"

void print_usage(char *argv[])
{
    printf("Usage: %s [options]\n", argv[0]);
    printf("\t -h - host where the server is running\n");
    printf("\t -p - the port the server is running on\n");
    printf("\t -a \"name,address,hours\" - new entry to add to db\n");
    printf("\t -d \"name\" - delete employee \"name\" from database file\n");
    printf("\t -l - list the employees currently in the file\n");
    printf("\t -u \"name\" -v hours - update employee \"name\" hours\n");
    return;
}

StatusCode start_client(int argc, char *argv[])
{
    int c = 0;

    ProgramOptions opts = {0};
    opts.ls_employees = false;
    opts.del = false;
    opts.update = false;
    opts.input_hours = -1;

    int port = 0;
    char *host = NULL;

    int status = STATUS_OK;

    char buf[4096] = {0};

    // Prologue
    while ((c = getopt(argc, argv, "a:ld:u:v:p:h:")) != -1)
    {
        switch (c)
        {
        case 'a':
            opts.newinput = strndup(optarg, DB_INPUT_MAX_SIZE - 2);
            if (!opts.newinput)
            {
                perror("strdup failed for newinput");
                return STATUS_MEMORY_ERROR;
            }
            break;
        case 'l':
            opts.ls_employees = true;
            break;
        case 'd':
            opts.search_name = strndup(optarg, DB_INPUT_MAX_SIZE - 2);
            if (!opts.search_name)
            {
                perror("strdup failed for search_name");
                return STATUS_MEMORY_ERROR;
            }
            opts.del = true;
            break;
        case 'u':
            opts.search_name = strndup(optarg, DB_INPUT_MAX_SIZE - 2);
            if (!opts.search_name)
            {
                perror("strdup failed for search_name");
                return STATUS_MEMORY_ERROR;
            }
            opts.update = true;
            break;
        case 'v':
            opts.input_hours = atoi(optarg);
            break;
        case 'h':
            host = strndup(optarg, DB_INPUT_MAX_SIZE - 2);
            break;
        case 'p':
            port = atoi(optarg);
            break;
        case '?':
            printf("Unknown argument -%c\n", c);
            print_usage(argv);
            return STATUS_INVALID_ARGUMENT;
        default:
            perror("getopt");
            return STATUS_INVALID_ARGUMENT;
        }
    }

    if (port == 0)
    {
        port = PORT;
    }

    if (host == NULL)
    {
        printf("Must specify host with -h\n");
        return STATUS_ERROR;
    }

    int socket = -1;
    status = create_conn_client_socket(&socket, host, port);
    if (status < 0)
    {
        printf("Error while creating client socket\n");
        return status;
    }

    status = send_hello(socket);
    if (status < 0)
    {
        printf("Error while sending hello request\n");
        close(socket);
        return status;
    }

    // State change operations
    if (status == STATUS_OK && opts.newinput)
    {
        printf("Client requesting add operation\n");
        send_employee(socket, opts.newinput);
    }

    if (status == STATUS_OK && opts.update)
    {
        // send update hours req
        status = send_update_hours(socket, opts.search_name, opts.input_hours);
        if (status < 0)
        {
            printf("Error while sending update hours request\n");
            close(socket);
            return status;
        }
    }

    if (status == STATUS_OK && opts.del)
    {
        // send delete req
        status = send_delete(socket, opts.search_name);
        if (status < 0)
        {
            printf("Error while sending delete request\n");
            close(socket);
            return status;
        }
    }

    // List operations
    if (status == STATUS_OK && opts.ls_employees)
    {
        printf("Client requesting list operation\n");
        status = send_list(socket);
        if (status < 0)
        {
            printf("Error while sending list request\n");
            close(socket);
            return status;
        }
    }

    close(socket);

    return STATUS_OK;
}