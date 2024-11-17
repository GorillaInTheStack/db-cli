#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include "common.h"
#include "parse.h"

void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees) 
{
	int i = 0;
	for(; i < dbhdr->count; i++)
	{
		printf("Employee %d\n", i);
		printf("\tName: %s\n", employees[i].name);
		printf("\tAddress: %s\n", employees[i].address);
		printf("\tHours: %d\n", employees[i].hours);
	}
}

int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring) 
{
	if (dbhdr == NULL)
	{
		printf("add_employee: Bad pointer to header struct\n");
		return STATUS_ERROR;
	}

	if (employees == NULL)
	{
		printf("add_employee: Bad pointer to employee struct\n");
		return STATUS_ERROR;
	}


	if (addstring == NULL)
	{
		printf("add_employee: Bad pointer to input char\n");
		return STATUS_ERROR;
	}

	char *name, *address, *hours = NULL;

	name = strtok(addstring, ",");
	address = strtok(NULL, ",");
	hours = strtok(NULL, ",");

	if (name == NULL || address == NULL || hours == NULL)
	{
		printf("add_employee: Malformed input\n");
		return STATUS_ERROR;
	}

	dbhdr->count++;
	struct employee_t *tmp_employees_ptr = NULL;

	tmp_employees_ptr = realloc(*employees, sizeof(struct employee_t) * dbhdr->count);
	if (tmp_employees_ptr == NULL)
	{
		perror("realloc");
		return STATUS_ERROR;
	}
	*employees = tmp_employees_ptr;
	tmp_employees_ptr = NULL;

	char *name_dest_ptr = strncpy((*employees)[dbhdr->count-1].name, name, sizeof((*employees)[dbhdr->count-1].name)-1);
	if (name_dest_ptr == NULL)
	{
		printf("add_employee: String copy of name was not successful\n");
		return STATUS_ERROR;
	}
	(*employees)[dbhdr->count-1].name[NAME_SIZE-1] = '\0';

	char *addr_dest_ptr = strncpy((*employees)[dbhdr->count-1].address, address, sizeof((*employees)[dbhdr->count-1].address)-1);
	if (addr_dest_ptr == NULL)
	{
		printf("add_employee: String copy of address was not successful\n");
		return STATUS_ERROR;
	}
	(*employees)[dbhdr->count-1].address[ADDRESS_SIZE-1] = '\0';
	
	(*employees)[dbhdr->count-1].hours = atoi(hours);

	return STATUS_OK;
}

int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut) 
{
	if (fd <= 0) 
	{
		printf("read_employees: Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	if (dbhdr == NULL)
	{
		printf("read_employees: Got bad pointer to header structi\n");
		return STATUS_ERROR;
	}

	if (employeesOut == NULL)
	{
		printf("read_employees: Got NULL pointer for employees\n");
		return STATUS_ERROR;
	}

	struct employee_t *tmp_employees_ptr = calloc(dbhdr->count, sizeof(struct employee_t));
	if (tmp_employees_ptr == NULL)
	{
		perror("calloc");
		return STATUS_ERROR;
	}

	int seek_bytes = lseek(fd, sizeof(struct dbheader_t), SEEK_SET);

	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		free(tmp_employees_ptr);
		return STATUS_ERROR;
	}
	
	int i = 0;
	for(; i < dbhdr->count; i++)
	{
		int bytes_read = read(fd, &tmp_employees_ptr[i], sizeof(struct employee_t));
		if (bytes_read == STATUS_ERROR)
		{
			perror("read");
			free(tmp_employees_ptr);
			return STATUS_ERROR;
		}
	}

	*employeesOut = tmp_employees_ptr;
	return STATUS_OK;
}

int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees) 
{
	if (fd <= 0) 
	{
		printf("output_file: Invalid file descriptor\n");
		return STATUS_ERROR;
	}

	if (dbhdr == NULL)
	{
		printf("output_file: Got bad pointer to header structi\n");
		return STATUS_ERROR;
	}

	if (employees == NULL)
	{
		printf("Writing empty database file\n");
	}
	
		int seek_bytes = lseek(fd, 0, SEEK_SET);
	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		return STATUS_ERROR;
	}

	dbhdr->magic = htonl(DB_MAGIC);
	dbhdr->version = htons(DB_CURRENT_VERSION);
	dbhdr->filesize = htonl(sizeof(struct dbheader_t) + (dbhdr->count * sizeof(struct employee_t)));
	unsigned short realcount = dbhdr->count;	
	dbhdr->count = htons(dbhdr->count);
	
	int write_bytes = write(fd, dbhdr, sizeof(struct dbheader_t));
	if (write_bytes == STATUS_ERROR)
	{
		perror("write");
		return STATUS_ERROR;
	}

	if(employees){
		int i = 0;
		for (; i < realcount; i++)
		{
			employees[i].hours = htonl(employees[i].hours);
			// convert the rest of them after testing
			write_bytes = write(fd, &employees[i], sizeof(struct employee_t));
			if (write_bytes == STATUS_ERROR)
			{
				perror("write");
				return STATUS_ERROR;
			}
		}
	}
	return STATUS_OK;
}	

int validate_db_header(int fd, struct dbheader_t **headerOut) 
{
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) 
	{
		perror("calloc");
		return STATUS_ERROR;
	}

	ssize_t bytes = read(fd, header, sizeof(struct dbheader_t));
	if (bytes == -1) 
	{
		perror("read");
		free(header);
		return STATUS_ERROR;
	}
	else if (bytes == 0) 
	{
		printf("validate_db_header: No bytes read\n");
		return STATUS_ERROR;
	}
	else if (bytes != sizeof(struct dbheader_t)) 
	{
		printf("validate_db_header: Not enough bytes read\n");
		free(header);
		return STATUS_ERROR;
	}

	header->magic = ntohl(header->magic);
	header->version = ntohs(header->version);
	header->count = ntohs(header->count);
	header->filesize = ntohl(header->filesize);

	if (header->magic != DB_MAGIC) 
	{
		printf("validate_db_header: Magic number mismatch\n");
		free(header);
		return STATUS_ERROR;
	}
	if (header->version != DB_CURRENT_VERSION) 
	{
		printf("validate_db_header: Version mismatch\n");
		free(header);
		return STATUS_ERROR;
	}

	struct stat db_stat = {0};
	fstat(fd, &db_stat);

	if (header->filesize != db_stat.st_size) 
	{
		printf("validate_db_header: File size mismatch\n");
		free(header);
		return STATUS_ERROR;
	}

	*headerOut = header;
	return STATUS_OK;

}

int create_db_header(int fd, struct dbheader_t **headerOut) 
{
	struct dbheader_t *header = calloc(1, sizeof(struct dbheader_t));
	if (header == NULL) 
	{
		perror("calloc");
		return STATUS_ERROR;
	}

	header->magic = DB_MAGIC;
	header->version = DB_CURRENT_VERSION;
	header->count = 0;
	header->filesize = sizeof(struct dbheader_t);
	
	*headerOut = header;
	return STATUS_OK;
}


