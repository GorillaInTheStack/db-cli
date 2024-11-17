#ifndef PARSE_H
#define PARSE_H

#define DB_MAGIC 0x4c4c4144
#define DB_CURRENT_VERSION 0x1
#define NAME_SIZE 256
#define ADDRESS_SIZE 256

struct dbheader_t {
	unsigned int magic;
	unsigned short version;
	unsigned short count;
	unsigned int filesize;
};

struct employee_t {
	char name[NAME_SIZE];
	char address[ADDRESS_SIZE];
	unsigned int hours;
};


/**
 * @brief create the header struct and populate it with initial values 
 *
 * @param fd database file descriptor
 * @param headerOut address to assign the created struct to
 *
 * @return 
 * 		- STATUS_OK if created successfully, struct is returned in *headerOut
 * 		- STATUS_ERROR if any error occurs
 */
int create_db_header(int fd, struct dbheader_t **headerOut);

/**
 * @brief Reads in the header of an existing database file and validates the values
 *
 * @param fd database file descriptor
 * @param headerOut address to assign the valided struct to
 *
 * @return 
 * 		- STATUS_OK if validated successfully, struct is returned in *headerOut
 * 		- STATUS_ERROR if any error occurs
 */
int validate_db_header(int fd, struct dbheader_t **headerOut);

/**
 * @brief 
 *
 * @param fd
 * @param 
 * @param employeesOut
 *
 * @return 
 */
int read_employees(int fd, struct dbheader_t *dbhdr, struct employee_t **employeesOut);

/**
 * @brief Writes the data structs to file after changes are made
 *
 * @param fd database file descriptor
 * @param dbhdr pointer to the dbheader_t struct
 * @param employees pointer to the employee_t struct
 *
 * @return 
 * 		- STATUS_OK if written to file successfully
 * 		- STATUS_ERROR if any error occurs
 */
int output_file(int fd, struct dbheader_t *dbhdr, struct employee_t *employees);

/**
 * @brief 
 *
 * @param dbhdr
 * @param employees
 */
void list_employees(struct dbheader_t *dbhdr, struct employee_t *employees);

/**
 * @brief 
 *
 * @param dbhdr
 * @param employees
 * @param addstring
 *
 * @return 
 */
int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring);

#endif
