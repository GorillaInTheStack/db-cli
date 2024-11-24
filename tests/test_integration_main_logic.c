#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>

#include "main_logic.h"
#include "context.h"
#include "file.h"
#include "parse.h"

static void test_process_arguments_newfile(void **state) {
    (void) state;

    char *test_file = "test.db";
    unlink(test_file);

    char *argv[] = {"test_program", "-n", "-f", test_file};
    optind = 1;
    int result = process_arguments(4, argv);

    assert_int_equal(result, STATUS_OK);

    FILE *file = fopen(test_file, "rb");
    assert_non_null(file);

    unsigned char buffer[16];
    size_t read_size = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    assert_int_equal(read_size, 16);
    assert_memory_equal(buffer, "LLAD\0\1\0\0\0\0\0\20\0\0\0\0", 16);

    unlink(test_file);
}

static void test_process_arguments_add_employee(void **state) {
    (void) state;

    char *test_file = "test.db";
    unlink(test_file);

    char *argv_create[] = {"test_program", "-n", "-f", test_file};
    optind = 1; // Reset getopt state to simulate new calls, otherwise subsequent calls will fail
    int result = process_arguments(4, argv_create);
    assert_int_equal(result, STATUS_OK);

    char *argv_add[] = {"test_program", "-f", test_file, "-a", "John Doe,123 Elm St.,40"};
    optind = 1;
    result = process_arguments(5, argv_add);
    assert_int_equal(result, STATUS_OK);

    FILE *file = fopen(test_file, "rb");
    assert_non_null(file);

    fseek(file, 16, SEEK_SET);  // Skip header
    unsigned char buffer[sizeof(Employee)];
    size_t read_size = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    assert_int_equal(read_size, sizeof(Employee));
    Employee *emp = (Employee *)buffer;
    assert_string_equal(emp->name, "John Doe");
    assert_string_equal(emp->address, "123 Elm St.");
    int hours = convert_uint(emp->hours, false);
    assert_int_equal(hours, 40);

    unlink(test_file);
}

static void test_process_arguments_list_employees(void **state) {
    (void) state;

    char *test_file = "test.db";
    unlink(test_file);

    char *argv_create[] = {"test_program", "-n", "-f", test_file};
    optind = 1;
    int result = process_arguments(4, argv_create);
    assert_int_equal(result, STATUS_OK);

    char *argv_add[] = {"test_program", "-f", test_file, "-a", "John Doe,123 Elm St.,40"};
    optind = 1;
    result = process_arguments(5, argv_add);
    assert_int_equal(result, STATUS_OK);

    char *argv_list[] = {"test_program", "-f", test_file, "-l"};
    optind = 1;
    result = process_arguments(4, argv_list);
    assert_int_equal(result, STATUS_OK);

    unlink(test_file);
}

static void test_process_arguments_delete_employee(void **state) {
    (void) state;

    char *test_file = "test.db";
    unlink(test_file);

    char *argv_create[] = {"test_program", "-n", "-f", test_file};
    optind = 1;
    int result = process_arguments(4, argv_create);
    assert_int_equal(result, STATUS_OK);

    char *argv_add[] = {"test_program", "-f", test_file, "-a", "John Doe,123 Elm St.,40"};
    optind = 1;
    result = process_arguments(5, argv_add);
    assert_int_equal(result, STATUS_OK);

    char *argv_delete[] = {"test_program", "-f", test_file, "-d", "John Doe"};
    optind = 1;
    result = process_arguments(5, argv_delete);
    assert_int_equal(result, STATUS_OK);

    unlink(test_file);
}

static void test_process_arguments_update_employee(void **state) {
    (void) state;

    char *test_file = "test.db";
    unlink(test_file);

    char *argv_create[] = {"test_program", "-n", "-f", test_file};
    optind = 1;
    int result = process_arguments(4, argv_create);
    assert_int_equal(result, STATUS_OK);

    char *argv_add[] = {"test_program", "-f", test_file, "-a", "John Doe,123 Elm St.,40"};
    optind = 1;
    result = process_arguments(5, argv_add);
    assert_int_equal(result, STATUS_OK);

    char *argv_update[] = {"test_program", "-f", test_file, "-u", "John Doe", "-v", "50"};
    optind = 1;
    result = process_arguments(7, argv_update);
    assert_int_equal(result, STATUS_OK);

    FILE *file = fopen(test_file, "rb");
    assert_non_null(file);

    fseek(file, 16, SEEK_SET);  // Skip header
    unsigned char buffer[sizeof(Employee)];
    size_t read_size = fread(buffer, 1, sizeof(buffer), file);
    fclose(file);

    assert_int_equal(read_size, sizeof(Employee));
    Employee *emp = (Employee *)buffer;
    int updated_hours = convert_uint(emp->hours, false);
    assert_int_equal(updated_hours, 50);

    unlink(test_file);
}


int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_process_arguments_newfile),
        cmocka_unit_test(test_process_arguments_add_employee),
        cmocka_unit_test(test_process_arguments_list_employees),
        cmocka_unit_test(test_process_arguments_delete_employee),
        cmocka_unit_test(test_process_arguments_update_employee)
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
