#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#include "context.h"
#include "parse.h"

int del_employee_by_name(DBContext *ctx, const char *name)
{
	StatusCode status = validate_context(ctx, true);
	if (status != STATUS_OK)
	{
		return status;
	}

	if (name == NULL)
	{
		printf("Error: Bad pointer to input char\n");
		return STATUS_INVALID_ARGUMENT;
	}

	// case: empty struct
	if (ctx->header->count == 0)
	{
		printf("Empty file, nothing to delete\n");
		return STATUS_OK;
	}

	// case: more than one struct
	bool found = false;
	size_t len = strnlen(name, NAME_SIZE);
	int i = 0;
	for (; i < ctx->header->count; i++)
	{
		if (found)
		{
			ctx->employees[i - 1] = ctx->employees[i];
		}
		else
		{
			if (strncmp(ctx->employees[i].name, name, NAME_SIZE) == 0)
			{
				found = true;
			}
		}
	}

	if (found)
	{
		Employee empty_employee = {0};
		// nullify the struct, not the pointer
		ctx->employees[ctx->header->count - 1] = empty_employee;
		if (ctx->header->count > 0)
		{
			ctx->header->count--;
		}
		return STATUS_OK;
	}
	else
	{
		return STATUS_EMPLOYEE_NOT_FOUND;
	}
}

int update_employee_hours_by_name(DBContext *ctx, const char *name, const int input_hours)
{
	StatusCode status = validate_context(ctx, true);
	if (status != STATUS_OK)
	{
		return status;
	}

	if (name == NULL)
	{
		printf("ERROR: Bad pointer to input char\n");
		return STATUS_INVALID_ARGUMENT;
	}

	if (input_hours < 0)
	{
		printf("Error: to update an employee with -u input the hours with flag -v [hours]\n");
		return STATUS_INVALID_ARGUMENT;
	}

	size_t len = strnlen(name, NAME_SIZE);
	int i = 0;
	for (; i < ctx->header->count; i++)
	{
		if (strncmp(ctx->employees[i].name, name, len) == 0)
		{
			ctx->employees[i].hours = input_hours;
			return STATUS_OK;
		}
	}

	return STATUS_EMPLOYEE_NOT_FOUND;
}

void list_employees(const DBContext *ctx)
{
	StatusCode status = validate_context(ctx, true);
	if (status != STATUS_OK)
	{
		printf("Invalid database context\n");
	}

	if (ctx->header->count == 0)
	{
		printf("No employees in file to list\n");
	}
	int i = 0;
	for (; i < ctx->header->count; i++)
	{
		printf("Employee %d\n", i);
		printf("\tName: %s\n", ctx->employees[i].name);
		printf("\tAddress: %s\n", ctx->employees[i].address);
		printf("\tHours: %d\n", ctx->employees[i].hours);
	}
}

int add_employee(DBContext *ctx, char *addstring)
{
	StatusCode status = validate_context(ctx, false);
	if (status != STATUS_OK)
	{
		return status;
	}

	if (addstring == NULL)
	{
		printf("Error: Bad pointer to input char\n");
		return STATUS_INVALID_ARGUMENT;
	}

	char *name, *address, *hours = NULL;

	name = strtok(addstring, ",");
	address = strtok(NULL, ",");
	hours = strtok(NULL, ",");

	if (name == NULL || address == NULL || hours == NULL)
	{
		printf("Error: Malformed input for add flag. Expected 'name,address,hours'\n");
		return STATUS_INVALID_ARGUMENT;
	}

	ctx->header->count++;
	Employee *tmp_employees_ptr = NULL;

	tmp_employees_ptr = realloc(ctx->employees, sizeof(Employee) * ctx->header->count);
	if (tmp_employees_ptr == NULL)
	{
		perror("realloc");
		return STATUS_MEMORY_ERROR;
	}
	ctx->employees = tmp_employees_ptr;
	tmp_employees_ptr = NULL;

	char *name_dest_ptr = strncpy(ctx->employees[ctx->header->count - 1].name, name, sizeof(ctx->employees[ctx->header->count - 1].name) - 1);
	if (name_dest_ptr == NULL)
	{
		printf("add_employee: String copy of name was not successful\n");
		return STATUS_ERROR;
	}
	ctx->employees[ctx->header->count - 1].name[NAME_SIZE - 1] = '\0';

	char *addr_dest_ptr = strncpy(ctx->employees[ctx->header->count - 1].address, address, sizeof(ctx->employees[ctx->header->count - 1].address) - 1);
	if (addr_dest_ptr == NULL)
	{
		printf("add_employee: String copy of address was not successful\n");
		return STATUS_ERROR;
	}
	ctx->employees[ctx->header->count - 1].address[ADDRESS_SIZE - 1] = '\0';

	ctx->employees[ctx->header->count - 1].hours = atoi(hours);

	return STATUS_OK;
}

int read_employees(DBContext *ctx)
{
	StatusCode status = validate_context(ctx, false);
	if (status != STATUS_OK)
	{
		return status;
	}

	if (ctx->db_fd <= 0)
	{
		printf("Error: Invalid file descriptor\n");
		return STATUS_INVALID_ARGUMENT;
	}

	Employee *tmp_employees_ptr = calloc(ctx->header->count, sizeof(Employee));
	if (tmp_employees_ptr == NULL)
	{
		perror("calloc");
		return STATUS_MEMORY_ERROR;
	}

	int seek_bytes = lseek(ctx->db_fd, sizeof(DBHeader), SEEK_SET);

	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		free(tmp_employees_ptr);
		return STATUS_SEEK_ERROR;
	}

	int i = 0;
	for (; i < ctx->header->count; i++)
	{
		int bytes_read = read(ctx->db_fd, &tmp_employees_ptr[i], sizeof(Employee));
		if (bytes_read == STATUS_ERROR)
		{
			perror("read");
			free(tmp_employees_ptr);
			return STATUS_READ_ERROR;
		}
		tmp_employees_ptr[i].hours = convert_uint(tmp_employees_ptr[i].hours, false);
		// TODO: when reading convert everything back once we modify so that everything is bigendian
	}

	ctx->employees = tmp_employees_ptr;
	tmp_employees_ptr = NULL;

	return STATUS_OK;
}

int output_file(DBContext *ctx)
{
	StatusCode status = validate_context(ctx, false);
	if (status != STATUS_OK)
	{
		return status;
	}
	if (ctx->db_fd <= 0)
	{
		printf("Error: Invalid file descriptor\n");
		return STATUS_INVALID_ARGUMENT;
	}

	int seek_bytes = lseek(ctx->db_fd, 0, SEEK_SET);
	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		return STATUS_SEEK_ERROR;
	}

	ctx->header->magic = convert_uint(DB_MAGIC, true);
	ctx->header->version = convert_ushort(DB_CURRENT_VERSION, true);
	size_t new_file_size = sizeof(DBHeader) + (ctx->header->count * sizeof(Employee));
	ctx->header->filesize = convert_uint(new_file_size, true);
	unsigned short realcount = ctx->header->count;
	ctx->header->count = convert_ushort(ctx->header->count, true);

	int write_bytes = write(ctx->db_fd, ctx->header, sizeof(DBHeader));
	if (write_bytes == STATUS_ERROR)
	{
		perror("write");
		return STATUS_WRITE_ERROR;
	}
	seek_bytes = lseek(ctx->db_fd, sizeof(DBHeader), SEEK_SET);
	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		return STATUS_SEEK_ERROR;
	}

	if (ctx->employees)
	{
		int i = 0;
		for (; i < realcount; i++)
		{
			ctx->employees[i].hours = convert_uint(ctx->employees[i].hours, true);
			// convert the rest of them after testing
			int write_bytes = write(ctx->db_fd, &(ctx->employees[i]), sizeof(Employee));
			if (write_bytes == STATUS_ERROR)
			{
				perror("write");
				return STATUS_WRITE_ERROR;
			}
			ctx->employees[i].hours = convert_uint(ctx->employees[i].hours, false);
		}
	}

	if (ftruncate(ctx->db_fd, new_file_size) == -1)
	{
		perror("ftruncate");
		return STATUS_ERROR;
	}

	ctx->header->magic = convert_uint(DB_MAGIC, false);
	ctx->header->version = convert_ushort(DB_CURRENT_VERSION, false);
	ctx->header->filesize = convert_uint(new_file_size, false);
	ctx->header->count = convert_ushort(ctx->header->count, false);

	return STATUS_OK;
}

int validate_db_header(DBContext *ctx)
{
	if (ctx == NULL)
	{
		printf("Error: Bad pointer to DBContext\n");
		return STATUS_INVALID_ARGUMENT;
	}
	DBHeader *header = calloc(1, sizeof(DBHeader));
	if (header == NULL)
	{
		perror("calloc");
		return STATUS_MEMORY_ERROR;
	}

	int seek_bytes = lseek(ctx->db_fd, 0, SEEK_SET);
	if (seek_bytes == STATUS_ERROR)
	{
		perror("lseek");
		return STATUS_SEEK_ERROR;
	}

	ssize_t bytes = read(ctx->db_fd, header, sizeof(DBHeader));
	if (bytes == -1)
	{
		perror("read");
		free(header);
		return STATUS_READ_ERROR;
	}
	else if (bytes == 0)
	{
		printf("Error: No bytes read\n");
		return STATUS_READ_ERROR;
	}
	else if (bytes != sizeof(DBHeader))
	{
		printf("Error: Not enough bytes read\n");
		free(header);
		return STATUS_WRITE_ERROR;
	}

	header->magic = convert_uint(header->magic, false);
	header->version = convert_ushort(header->version, false);
	header->count = convert_ushort(header->count, false);
	header->filesize = convert_uint(header->filesize, false);

	if (header->magic != DB_MAGIC)
	{
		printf("Error: Magic number mismatch\n");
		free(header);
		return STATUS_DB_CORRUPTED;
	}
	if (header->version != DB_CURRENT_VERSION)
	{
		printf("Error: Version mismatch\n");
		free(header);
		return STATUS_DB_CORRUPTED;
	}

	struct stat db_stat = {0};
	fstat(ctx->db_fd, &db_stat);

	if (header->filesize != db_stat.st_size)
	{
		printf("Error: File size mismatch\n");
		free(header);
		return STATUS_DB_CORRUPTED;
	}

	ctx->header = header;
	header = NULL;
	return STATUS_OK;
}

int create_db_header(DBContext *ctx)
{
	if (ctx == NULL)
	{
		printf("Error: Bad pointer to DBContext\n");
		return STATUS_INVALID_ARGUMENT;
	}
	DBHeader *header = calloc(1, sizeof(DBHeader));
	if (header == NULL)
	{
		perror("calloc");
		return STATUS_MEMORY_ERROR;
	}

	header->magic = DB_MAGIC;
	header->version = DB_CURRENT_VERSION;
	header->count = 0;
	header->filesize = sizeof(DBHeader);

	ctx->header = header;
	header == NULL;
	return STATUS_OK;
}
