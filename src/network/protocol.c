#include <string.h>

#include "protocol.h"
#include "common.h"

StatusCode send_hello(int fd)
{
    char buf[4096] = {0};

    DBProtoHeader *hdr = (DBProtoHeader *)buf;
    hdr->type = MSG_HELLO_REQ;
    hdr->len = 1;

    DBProtoHelloReq *hello = (DBProtoHelloReq *)&hdr[1];
    hello->proto = PROTO_VER;

    hdr->type = convert_uint(hdr->type, true);
    hdr->len = convert_uint(hdr->len, true);
    hello->proto = convert_ushort(hello->proto, true);

    int bytes = write(fd, buf, sizeof(DBProtoHeader) + sizeof(DBProtoHelloReq));
    if (bytes <= 0)
    {
        perror("write");
        return STATUS_WRITE_ERROR;
    }

    int status = read(fd, buf, sizeof(buf));
    if (status <= 0)
    {
        printf("Failed to read from server.\n");
        return STATUS_ERROR;
    }

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_ushort(hdr->len, false);

    if (hdr->type == MSG_ERROR)
    {
        printf("Protocol mistmatch\n");
        return STATUS_ERROR;
    }

    printf("Sent hello request to server -> protocol v%d\n", PROTO_VER);
    return STATUS_OK;
}

StatusCode send_employee(int fd, char *employee_str)
{
    char buf[4096] = {0};

    DBProtoHeader *hdr = (DBProtoHeader *)buf;
    hdr->type = MSG_EMPLOYEE_ADD_REQ;
    hdr->len = 1;

    DBProtoEmployeeAddReq *employee = (DBProtoEmployeeAddReq *)&hdr[1];
    strncpy((char *)&employee->data, employee_str, sizeof(employee->data));

    hdr->type = convert_uint(hdr->type, true);
    hdr->len = convert_uint(hdr->len, true);

    int bytes = write(fd, buf, sizeof(DBProtoHeader) + sizeof(DBProtoEmployeeAddReq));
    if (bytes <= 0)
    {
        perror("write");
        return STATUS_WRITE_ERROR;
    }

    int status = read(fd, buf, sizeof(buf));
    if (status <= 0)
    {
        printf("Failed to read from server.\n");
        return STATUS_ERROR;
    }

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_ushort(hdr->len, false);

    if (hdr->type == MSG_ERROR)
    {
        printf("Server error when adding employee\n");
        return STATUS_ERROR;
    }

    if (hdr->type == MSG_EMPLOYEE_ADD_RESP)
    {
        printf("Employee added\n");
    }

    return STATUS_OK;
}

StatusCode send_list(int fd)
{
    char buf[4096] = {0};

    DBProtoHeader *hdr = (DBProtoHeader *)buf;
    hdr->type = MSG_EMPLOYEE_LIST_REQ;
    hdr->len = 0;

    hdr->type = convert_uint(hdr->type, true);
    hdr->len = convert_uint(hdr->len, true);

    int bytes = write(fd, buf, sizeof(DBProtoHeader));
    if (bytes <= 0)
    {
        perror("write");
        return STATUS_WRITE_ERROR;
    }

    int status = read(fd, buf, sizeof(buf));
    if (status <= 0)
    {
        printf("Failed to read from server.\n");
        return STATUS_ERROR;
    }

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_ushort(hdr->len, false);

    if (hdr->type == MSG_ERROR)
    {
        printf("Server error when listing employees\n");
        return STATUS_ERROR;
    }

    // convert payload and show
    if (hdr->type == MSG_EMPLOYEE_LIST_RESP)
    {
        printf("Listing employees...\n");
        Employee *employee = (Employee *)&hdr[1];

        for (int i = 0; i < hdr->len; i++)
        {
            read(fd, employee, sizeof(Employee));
            employee->hours = convert_uint(employee->hours, false);
            printf("%s, %s, %d\n", employee->name, employee->address, employee->hours);
        }
    }
    printf("Done.\n");
    return STATUS_OK;
}

StatusCode send_update_hours(int fd, char *name, int hours_input)
{
    char buf[4096] = {0};

    DBProtoHeader *hdr = (DBProtoHeader *)buf;
    hdr->type = MSG_EMPLOYEE_UPDATE_REQ;
    hdr->len = 2;
    int *hours = (int *)&hdr[1];
    char *name_tosend = (char *)&hdr[2];

    *hours = hours_input;
    strncpy(name_tosend, name, sizeof(name_tosend));
    name_tosend[NAME_SIZE - 1] = '\0';

    hdr->type = convert_uint(hdr->type, true);
    hdr->len = convert_uint(hdr->len, true);

    int bytes = write(fd, buf, sizeof(DBProtoHeader) + sizeof(name_tosend) + sizeof(int));
    if (bytes <= 0)
    {
        perror("write");
        return STATUS_WRITE_ERROR;
    }

    int status = read(fd, buf, sizeof(buf));
    if (status <= 0)
    {
        printf("Failed to read from server.\n");
        return STATUS_ERROR;
    }

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_ushort(hdr->len, false);

    if (hdr->type == MSG_ERROR)
    {
        printf("Server error when updating employee\n");
        return STATUS_ERROR;
    }

    if (hdr->type == MSG_EMPLOYEE_DEL_RESP)
    {
        printf("Employee updated\n");
    }

    return STATUS_OK;
}

StatusCode send_delete(int fd, char *name)
{
    char buf[4096] = {0};

    DBProtoHeader *hdr = (DBProtoHeader *)buf;
    hdr->type = MSG_EMPLOYEE_DEL_REQ;
    hdr->len = 1;

    char *name_tosend = (char *)&hdr[1];

    strncpy(name_tosend, name, sizeof(name_tosend));
    name_tosend[NAME_SIZE - 1] = '\0';

    hdr->type = convert_uint(hdr->type, true);
    hdr->len = convert_uint(hdr->len, true);

    int bytes = write(fd, buf, sizeof(DBProtoHeader) + sizeof(name_tosend));
    if (bytes <= 0)
    {
        perror("write");
        return STATUS_WRITE_ERROR;
    }

    int status = read(fd, buf, sizeof(buf));
    if (status <= 0)
    {
        printf("Failed to read from server.\n");
        return STATUS_ERROR;
    }

    hdr->type = convert_uint(hdr->type, false);
    hdr->len = convert_ushort(hdr->len, false);

    if (hdr->type == MSG_ERROR)
    {
        printf("Server error when deleting employee\n");
        return STATUS_ERROR;
    }

    if (hdr->type == MSG_EMPLOYEE_DEL_RESP)
    {
        printf("Employee deleted\n");
    }

    return STATUS_OK;
}
