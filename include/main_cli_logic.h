#ifndef MAIN_CLI_LOGIC_H
#define FILE_H

/**
 * @brief prints out the CLI app usage message
 *
 * @param argv[] arguments vector
 */
void print_usage(char *argv[]);

/**
 * @brief contains the main logic controller of the CLI app
 *
 * @param argc arguments count
 * @param argv[] arguments vector
 *
 * @return int -> StatusCode
 */
int process_arguments(int argc, char *argv[]);

#endif
