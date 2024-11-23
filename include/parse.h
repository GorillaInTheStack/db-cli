#ifndef PARSE_H
#define PARSE_H

#define DB_MAGIC 0x4c4c4144
#define DB_CURRENT_VERSION 0x1


	/**
	 * @brief create the header struct and populate it with initial values 
	 *
	 * @param ctx Pointer to the DBContext containing the file descriptor and header.
	 *
	 * @return 
	 * 		- STATUS_OK if created successfully, and header is returned in ctx->header
	 * 		- STATUS_ERROR if any error occurs
	 */
	int create_db_header(DBContext *ctx);

	/**
	 * @brief Reads in the header of an existing database file and validates the values.
	 *
	 * @param ctx Pointer to the DBContext containing the file descriptor and header.
	 *
	 * @return
	 *      - STATUS_OK if validated successfully, and the header is updated in ctx->header.
	 *      - STATUS_ERROR if any error occurs.
	 */
	int validate_db_header(DBContext *ctx);


	/**
	 * @brief Parses the data belonging to the Employee struct in the database file.
	 *
	 * @param ctx Pointer to the DBContext containing the file descriptor and header.
	 *
	 * @return
	 *      - STATUS_OK on successful extraction of Employee data, stored in ctx->employees.
	 *      - STATUS_ERROR on any failure.
	 */
	int read_employees(DBContext *ctx);

	/**
	 * @brief Writes the header and employee data to the database file after changes are made.
	 *
	 * @param ctx Pointer to the DBContext containing the file descriptor, header, and employees.
	 *
	 * @return
	 *      - STATUS_OK if written to file successfully.
	 *      - STATUS_ERROR if any error occurs.
	 */
	int output_file(DBContext *ctx);

	/**
	 * @brief Lists the employee data currently available in memory.
	 *
	 * @param ctx Pointer to the DBContext containing the header and employees.
	 */
	void list_employees(const DBContext *ctx);

	/**
	 * @brief Adds a new employee to the database, provided as an input string.
	 *
	 * @param ctx Pointer to the DBContext containing the header and employee data.
	 * @param addstring New employee data to be added in the format "Name,Address,Hours" (e.g., "Sam Smile,123 Life av.,682").
	 *
	 * @return
	 *      - STATUS_OK if the employee was successfully added.
	 *      - STATUS_ERROR in case of any failure.
	 */
	int add_employee(DBContext *ctx, char *addstring);

   /**
 	* @brief Deletes an employee from the database by their name.
 	*
 	* @param ctx Pointer to the DBContext containing the header and employee data.
 	* @param name The name of the employee to delete.
 	*
 	* @return
 	*      - STATUS_OK if the employee was found and deleted.
 	*      - STATUS_ERROR in case of any failure (e.g., not found).
 	*/
	int del_employee_by_name(DBContext *ctx, const char *name);

   /**
 	* @brief Updates the hours worked for an employee, identified by their name.
 	*
 	* @param ctx Pointer to the DBContext containing the header and employee data.
 	* @param name The name of the employee whose hours need to be updated.
 	* @param hours The new number of hours to set for the employee.
 	*
 	* @return
 	*      - STATUS_OK if the employee was found and updated.
 	*      - STATUS_ERROR in case of any failure (e.g., not found).
 	*/
	int update_employee_hours_by_name(DBContext *ctx, const char *name, const int hours);

#endif
