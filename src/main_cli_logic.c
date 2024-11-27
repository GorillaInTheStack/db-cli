#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

#include "context.h"
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

int process_arguments(int argc, char *argv[])
{
	int c = 0;

	ProgramOptions opts = {0};
	opts.newfile = false;
	opts.ls_employees = false;
	opts.del = false;
	opts.update = false;
	opts.input_hours = -1;

	DBContext ctx = {0};
	ctx.db_fd = -1;

	int status = STATUS_OK;

	// Prologue
	while ((c = getopt(argc, argv, "nf:a:ld:u:v:")) != -1) {
		switch (c) {
			case 'n':
				opts.newfile = true;
				break;
			case 'f':
				// we should not depend on optarg address after getopt returns
				// as it may be overwritten by subsequent calls (during tests) to getopt
				// so we copy the string to a new memory location
				opts.filepath = strndup(optarg, DB_INPUT_MAX_SIZE-2);
				if (!opts.filepath) {
					perror("strdup failed for filepath");
					return STATUS_MEMORY_ERROR;
				}
				break;
			case 'a':
				opts.newinput = strndup(optarg, DB_INPUT_MAX_SIZE-2);
				if (!opts.newinput) {
					perror("strdup failed for newinput");
					return STATUS_MEMORY_ERROR;
				}
				break;
			case 'l':
				opts.ls_employees = true;
				break;
			case 'd':
				opts.search_name = strndup(optarg, DB_INPUT_MAX_SIZE-2);
				if (!opts.search_name) {
					perror("strdup failed for search_name");
					return STATUS_MEMORY_ERROR;
				}
				opts.del = true;
				break;
			case 'u':
				opts.search_name = strndup(optarg, DB_INPUT_MAX_SIZE-2);
				if (!opts.search_name) {
					perror("strdup failed for search_name");
					return STATUS_MEMORY_ERROR;
				}
				opts.update = true;
				break;
			case 'v':
				opts.input_hours = atoi(optarg);
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

	if(opts.filepath == NULL){
		printf("Path is a required argument\n");
		print_usage(argv);
		return STATUS_FILE_NOT_FOUND;
	}

	// File handling	
	if (status == STATUS_OK && opts.newfile)
	{
		status = create_db_file(opts.filepath, &ctx.db_fd);
		if(status == STATUS_OK)
		{
			status = create_db_header(&ctx);
		}
		if (status == STATUS_FILE_EXISTS)
		{
			printf("File already exists\n");
			status = STATUS_OK; // continue with existing file
			opts.newfile = false;
		}
	} 
	
	if (status == STATUS_OK && opts.filepath && !opts.newfile)
	{
		status = open_db_file(opts.filepath, &ctx.db_fd);
		if (status == STATUS_OK)
		{
			status = validate_db_header(&ctx);
		}
		if (status == STATUS_OK )
		{
	    	status = read_employees(&ctx);
		}
	}	

	// State change operations
	if (status == STATUS_OK && opts.newinput)
	{
		status = add_employee(&ctx, opts.newinput);	
		if (status == STATUS_OK)
		{
			printf("Employee %s added.\n", opts.newinput);
		}
	}

	if (status == STATUS_OK && opts.update)
	{
		status = update_employee_hours_by_name(&ctx, opts.search_name, opts.input_hours);
		if (status == STATUS_OK)
		{
			printf("Employee %s hours updated.\n", opts.search_name);
		}
	}

	if (status == STATUS_OK && opts.del)
	{
		int status = del_employee_by_name(&ctx, opts.search_name);
		if (status == STATUS_OK)
		{
			printf("Employee %s deleted.\n", opts.search_name);
		}
	}

	// List operations
	if (status == STATUS_OK && opts.ls_employees)
	{
		if (ctx.employees == NULL)
		{
			printf("No employees in database to list.\n");
		}else
		{
			list_employees(&ctx);
		}
	}

	// Epilogue
	if (status == STATUS_OK)
	{
		status = output_file(&ctx); 
	}

	cleanup_context(&ctx);

	if (opts.filepath) free(opts.filepath);
	if (opts.newinput) free(opts.newinput);
	if (opts.search_name) free(opts.search_name);

	return status;
}
