#ifndef FILE_H
#define FILE_H

/**
 * @brief Creates a database file using filename
 *
 * @param filename: char*
 *
 * @return 
 * 		- fd if successful
 * 		- STATUS_ERROR if not successful
 */
int create_db_file(char *filename);

/**
 * @brief Open a database file named "filename" if it exits
 *
 * @param filename: char*
 *
 * @return 
 *		- fd if successful
 *		- STATUS_ERROR if not successful
 */
int open_db_file(char *filename);

#endif
