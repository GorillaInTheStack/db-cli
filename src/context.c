#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "context.h"

void cleanup_context(DBContext *ctx) {
    if (ctx->header) free(ctx->header);
    if (ctx->employees) free(ctx->employees);
    if (ctx->db_fd > 0) close(ctx->db_fd);
    memset(ctx, 0, sizeof(DBContext)); 
	ctx = NULL;
}

StatusCode validate_context(const DBContext *ctx, bool check_employees) {
    if (ctx == NULL) {
        printf("Error: Invalid DBContext pointer\n");
        return STATUS_INVALID_ARGUMENT;
    }

    if (ctx->header == NULL) {
        printf("Error: DBContext header is null\n");
        return STATUS_INVALID_ARGUMENT;
    }

    if (check_employees && ctx->employees == NULL) {
        printf("Error: DBContext employees is null\n");
        return STATUS_INVALID_ARGUMENT;
    }

    return STATUS_OK;
}

unsigned int convert_uint(unsigned int val, bool to_network) {
    return to_network ? htonl(val) : ntohl(val);
}

unsigned short convert_ushort(unsigned short val, bool to_network) {
    return to_network ? htons(val) : ntohs(val);
}

const char* status_message(StatusCode status) {
    switch (status) {
        case STATUS_OK: 
            return "Operation successful.";
        case STATUS_ERROR: 
            return "General error.";
        case STATUS_INVALID_ARGUMENT: 
            return "Invalid argument.";
        case STATUS_FILE_NOT_FOUND: 
            return "File not found.";
        case STATUS_FILE_EXISTS: 
            return "File already exists.";
        case STATUS_READ_ERROR: 
            return "Error while reading the file.";
        case STATUS_WRITE_ERROR: 
            return "Error while writing to the file.";
        case STATUS_SEEK_ERROR: 
            return "Error while seeking in the file.";
        case STATUS_FILE_OPEN_ERROR: 
            return "Error opening the file.";
        case STATUS_FILE_CREATE_ERROR: 
            return "Error creating the file.";
        case STATUS_MEMORY_ERROR: 
            return "Memory allocation failed.";
        case STATUS_DB_CORRUPTED: 
            return "Database corrupted.";
        case STATUS_DB_EMPTY: 
            return "Database is empty.";
        case STATUS_EMPLOYEE_NOT_FOUND: 
            return "Employee not found.";
        default: 
            return "Unknown status code.";
    }
}

