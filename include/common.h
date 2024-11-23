#ifndef COMMON_H
#define COMMON_H

#include <arpa/inet.h>
#include <stdbool.h>

// 256 + 256 + 11 + 2 + 1 = 526 bytes -> 11 for the hours(char), 2 for the commas, 1 for the null terminator
#define DB_INPUT_MAX_SIZE (NAME_SIZE + ADDRESS_SIZE + 11 + 2 + 1)
#define NAME_SIZE 256
#define ADDRESS_SIZE 256

typedef enum {
    // General status codes
    STATUS_OK = 0,               
    STATUS_ERROR = -1,          

    // Argument validation
    STATUS_INVALID_ARGUMENT = -2,

    // File-related errors
    STATUS_FILE_NOT_FOUND = -3,   
    STATUS_FILE_EXISTS = -4,    
    STATUS_READ_ERROR = -5,      
    STATUS_WRITE_ERROR = -6,      
    STATUS_SEEK_ERROR = -7,      
	STATUS_FILE_OPEN_ERROR = -8,
	STATUS_FILE_CREATE_ERROR = -9,

    // Memory errors
    STATUS_MEMORY_ERROR = -10, 

    // Database-specific errors
    STATUS_DB_CORRUPTED = -11, 
    STATUS_DB_EMPTY = -12, 
    STATUS_EMPLOYEE_NOT_FOUND = -13 
} StatusCode;

const char* status_message(StatusCode status);

typedef struct {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	size_t filesize;
} DBHeader;

typedef struct {
	char name[NAME_SIZE];
	char address[ADDRESS_SIZE];
	unsigned int hours;
} Employee;

typedef struct {
    bool newfile;         // -n flag
    char *filepath;       // -f argument
    char *newinput;       // -a argument
    bool ls_employees;    // -l flag
    char *search_name;    // -d or -u argument
    bool del;         	  // -d flag
    bool update;          // -u flag
    int input_hours;      // -v argument
} ProgramOptions;

/**
 * @brief Converts a 32-bit unsigned integer between host and network byte order.
 *
 * @param val The value to convert.
 * @param to_network If true, converts from host to network order. If false, converts from network to host order.
 * @return The converted 32-bit unsigned integer.
 */
unsigned int convert_uint(unsigned int val, bool to_network);

/**
 * @brief Converts a 16-bit unsigned integer between host and network byte order.
 *
 * @param val The value to convert.
 * @param to_network If true, converts from host to network order. If false, converts from network to host order.
 * @return The converted 16-bit unsigned integer.
 */
unsigned short convert_ushort(unsigned short val, bool to_network);

#endif
