#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "file.h"
#include "context.h"

StatusCode create_db_file(const char *filename, int *fd) {
    if (!filename || !fd) {
        return STATUS_INVALID_ARGUMENT;
    }

    int tmp_fd = open(filename, O_RDWR, 0644);
    if (tmp_fd != -1) {
        close(tmp_fd);
        return STATUS_FILE_EXISTS; 
    }

    tmp_fd = open(filename, O_CREAT | O_RDWR, 0644);
    if (tmp_fd == -1) {
        perror("open");
        return STATUS_FILE_CREATE_ERROR; 
    }

    *fd = tmp_fd;
    return STATUS_OK; 
}

StatusCode open_db_file(const char *filename, int *fd) {
    if (!filename || !fd) {
        return STATUS_INVALID_ARGUMENT;
    }

    int tmp_fd = open(filename, O_RDWR, 0644);
    if (tmp_fd == -1) {
        perror("open");
        return STATUS_FILE_OPEN_ERROR;
    }

    *fd = tmp_fd;
    return STATUS_OK; 
}

