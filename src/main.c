#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) 
{
	printf("Usage: %s -n -f <db file>\n", argv[0]);
	printf("\t -n - create new db file\n");
	printf("\t -f - (required) path to db file\n");
	printf("\t -a - new entry to add to db\n");
	return;
}

int main(int argc, char *argv[]) 
{ 
	int c = 0;
	bool newfile = false;
	char *filepath = NULL;
	char *newinput = NULL;
	int db_fd = -1;
	struct dbheader_t *db_hdr = NULL;
	struct employee_t *employees_ptr = NULL; 
	bool ls_employees = false;

	while ((c= getopt(argc, argv, "nf:a:l")) != -1) 
	{
			switch(c)
			{
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				newinput = optarg;
				break;
			case 'l':
				ls_employees = true;
				break;
			case '?':
				printf("Unknown argument -%c\n", c);
				print_usage(argv);
				break;
			default:
				perror("getopt");
				return STATUS_ERROR;	
			}
	}

	if(filepath == NULL){
		printf("Filepath is a required argument\n");
		print_usage(argv);
		return STATUS_ERROR;
	}
	
	if (newfile)
	{
		db_fd = create_db_file(filepath);
		if(db_fd == STATUS_ERROR)
		{
			printf("Unable to create database file\n");
			return STATUS_ERROR;
		}
		//TODO: when -l -n -a is given, this might create a new header by mistake if file already exists
		if (create_db_header(db_fd, &db_hdr) == STATUS_ERROR)
		{
			printf("Failed to create database header\n");
			close(db_fd);
			return STATUS_ERROR;
		}
	} else
	{ 
		db_fd = open_db_file(filepath);
		if (db_fd == STATUS_ERROR)
		{
			printf("Unable to open database file\n");
			return STATUS_ERROR;
		}
		if (validate_db_header(db_fd, &db_hdr) == STATUS_ERROR)
		{
			printf("Failed to validate DB header\n");
			close(db_fd);
			return STATUS_ERROR;
		}
		int status = read_employees(db_fd, db_hdr, &employees_ptr);
		if (status == STATUS_ERROR)
		{
			printf("read_employees: Error reading existing employees\n");
			return STATUS_ERROR;
		}
	}

	if (newinput)
	{
		add_employee(db_hdr, &employees_ptr, newinput);	

		if(employees_ptr == NULL)
		{
			perror("calloc");
			close(db_fd);
			free(db_hdr);
			db_hdr = NULL;
			return STATUS_ERROR;
		}
	}

	if (ls_employees)
	{
		if (employees_ptr == NULL)
		{
			printf("No employees in database to list.\n");
		}else
		{
			list_employees(db_hdr, employees_ptr);
		}

	}
	//TODO: something about bad file descriptor comes from here with -n -l -a
	output_file(db_fd, db_hdr, employees_ptr); 

	free(db_hdr);
	db_hdr = NULL;
	free(employees_ptr);
	employees_ptr = NULL;
	if (newinput)
	{
		//free(newinput);
		newinput = NULL;
	}
	if (filepath)
	{
		//free(filepath);
		filepath = NULL;
	}
	return STATUS_OK;
}
