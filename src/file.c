#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include "file.h"
#include "common.h"


int create_db_file(char *filename) 
{
	int fd = open(filename, O_RDWR, 0644);
	if (fd != -1) {
		close(fd);
		printf("File already exists\n");
		return fd;
	}
	else {
		close(fd);
		fd = open(filename, O_CREAT | O_RDWR, 0644);
		if (fd == -1) {
			perror("open");
			return STATUS_ERROR;
		}
		else {
			printf("File created\n");
			return fd;
		}
	}
}

int open_db_file(char *filename)
{	
	int fd = open(filename, O_CREAT | O_RDWR, 0644);
	if (fd == -1) {
		close(fd);
		perror("open");
		return STATUS_ERROR;
	}
	else {
		return fd;
	}
}

