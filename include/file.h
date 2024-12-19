#ifndef FILE_H
#define FILE_H

#include "common.h"

/**
 * @brief Creates a database file using filename
 *
 * @param filename: char*
 *
 * @return
 * 		- STATUS_OK if successful
 * 		- STATUS_ERROR if not successful
 */
StatusCode create_db_file(const char *filename, int *fd);

/**
 * @brief Open a database file named "filename" if it exits
 *
 * @param filename: char*
 *
 * @return
 *		- fd if successful
 *		- STATUS_ERROR if not successful
 */
StatusCode open_db_file(const char *filename, int *fd);

#endif
