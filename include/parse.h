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
int create_db_header(const int fd, struct dbheader_t **headerOut);

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
 * @brief Parses the data belonging to employee_t struct in the database file 
 *
 * @param fd database file descriptor
 * @param dbhdr database file header data
 * @param employeesOut returned employee_t data
 *
 * @return 
 * 		- STATUS_OK on successful extraction of employee_t in *employeesOut
 * 		- STATUS_ERROR on any failure
 */
int read_employees(int fd, const struct dbheader_t *dbhdr, struct employee_t **employeesOut);

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
 * @brief Lists the employee_t data currently available in in memory
 *
 * @param dbhdr database file header data
 * @param employees employee data
 */
void list_employees(const struct dbheader_t *dbhdr, const struct employee_t *employees);

/**
 * @brief Adds a new employee to the database file which is inputted in the form of a string
 *
 * @param dbhdr database file header data
 * @param employees current employees data
 * @param addstring new employee to be added. (ex. "Sam Smile,123 Life av.,682")
 *
 * @return
 * 		- STATUS_OK if employees was updated successfully
 * 		- STATUS_ERROR in case of any failure
 */
int add_employee(struct dbheader_t *dbhdr, struct employee_t **employees, char *addstring);

/**
 * @brief Find an employee by the name given in the list of employees
 *
 * @param dbhdr database file header data
 * @param employees list of current employees
 * @param name name to search for
 * @param employeeOut returned employee
 *
 * @return
 * 		- STATUS_OK if found and returned in **employeeOut
 * 		- STATUS_ERROR in case of any failure
 */
int find_employee_by_name(const struct dbheader_t *dbhdr, const struct employee_t *employees, const char *name, struct employee_t **employeeOut);

/**
 * @brief Delete an employee by name
 *
 * @param dbhdr database file header data
 * @param employees list of current employees
 * @param name the name to search for
 *
 * @return 
 * 		- STATUS_OK if found and deleted in **employees
 * 		- STATUS_ERROR in case of any failure
 */
int del_employee_by_name(struct dbheader_t *dbhdr, struct employee_t **employees, const char *name);

/**
 * @brief update employee hours by name
 *
 * @param dbhdr database file header data
 * @param employees list of current employees
 * @param name the name to search for
 *
 * @return 
 * 		- STATUS_OK if found and hours updated in **employees
 * 		- STATUS_ERROR in case of any failure
 */
int update_employee_hours_by_name(const struct dbheader_t *dbhdr, struct employee_t **employees, const char *name);

#endif
