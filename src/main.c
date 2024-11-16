#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) 
{
	printf("Usage: %s -n -f <db file>\n", argv[0]);
	printf("\t -n - create new db file\n");
	printf("\t -f - (required) path to db file\n");
	return;
}

int main(int argc, char *argv[]) 
{ 
	int c = 0;
	bool newfile = false;
	char *filepath = NULL;

	while ((c= getopt(argc, argv, "nf:") != -1)) 
	{
			switch(c)
			{
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case '?':
				printf("Unkown argument -%c\n", c);
				print_usage(argv);
				break;
			default:
				return -1;	
			}
	}

	if(filepath == NULL){
		printf("Filepath is a required argument\n");
		print_usage(argv);
		return -1;
	}

	return 0;

}
