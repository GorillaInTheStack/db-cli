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
	printf("\t -f \"path\" - (required) path to db file\n");
	printf("\t -a \"name,address,hours\" - new entry to add to db\n");
	printf("\t -d \"name\" - delete employee \"name\" from database file\n");
	printf("\t -u \"name\" -v hours - update employee \"name\" hours\n");
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
	char *search_name = NULL;
	bool delete = false;
	bool update = false;
	int input_hours = -1;

	while ((c= getopt(argc, argv, "nf:a:ld:u:v:")) != -1) 
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
				newinput[DB_INPUT_MAX_SIZE-1] = '\0';
				break;
			case 'l':
				ls_employees = true;
				break;
			case 'd':
				search_name = optarg;
				search_name[NAME_SIZE-1] = '\0';
				delete = true;
				break;
			case 'u':
				search_name = optarg;
				search_name[NAME_SIZE-1] = '\0';
				update = true;
				break;
			case 'v':
				input_hours = atoi(optarg);
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
		printf("Path is a required argument\n");
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

			if (db_fd > 0)
			{
				close(db_fd);
			}
			if (db_hdr)
			{
				free(db_hdr);
				db_hdr = NULL;
			}
			if (employees_ptr)
			{
				free(employees_ptr);
				employees_ptr = NULL;
			}

			return STATUS_ERROR;
		}
	}

	if (newinput)
	{
		if(employees_ptr == NULL)
		{
			printf("There seems to be a problem with the database file\n");
			close(db_fd);
			if (db_hdr)
			{
				free(db_hdr);
				db_hdr = NULL;
			}
			return STATUS_ERROR;
		}

		int status = add_employee(db_hdr, &employees_ptr, newinput);	
		if (status == STATUS_ERROR)
		{
			printf("Failed to add employee\n");

			if (db_fd > 0)
			{
				close(db_fd);
			}
			if (db_hdr)
			{
				free(db_hdr);
				db_hdr = NULL;
			}
			if (employees_ptr)
			{
				free(employees_ptr);
				employees_ptr = NULL;
			}

			return STATUS_ERROR;
		}

	}

	if (update)
	{
		if (input_hours < 0)
		{
			printf("Error: to update an employee with -u input the hours with flag -v [hours]\n");
			return STATUS_ERROR;
		}
		int status = update_employee_hours_by_name(db_hdr, employees_ptr, search_name, input_hours);
		if (status == STATUS_OK)
		{
			printf("Employee hours updated.\n");
		}
		else
		{
			printf("There was a problem updating the employee.\n");
			return STATUS_ERROR;
		}
	}

	if (delete)
	{
		int status = del_employee_by_name(db_hdr, &employees_ptr, search_name);
		if (status == STATUS_OK)
		{
			printf("Employee deleted.\n");
		}
		else
		{
			printf("There was a problem deleting the employee.\n");
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

	if(db_hdr)
	{
		free(db_hdr);
		db_hdr = NULL;
	}
	if (employees_ptr)
	{
		free(employees_ptr);
		employees_ptr = NULL;
	}

	return STATUS_OK;
}
