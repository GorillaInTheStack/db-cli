#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>   
#include "file.h"    

#define MAX_FD 1024

static void test_create_db_file_success(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    unlink(test_file);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
	assert_in_range(fd, 0, MAX_FD);

    close(fd);
    unlink(test_file);
}

static void test_create_db_file_exists(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    create_db_file(test_file, &fd);
    close(fd);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_FILE_EXISTS);
	assert_in_range(fd, 0, MAX_FD);

    unlink(test_file);
}

static void test_open_db_file_success(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    create_db_file(test_file, &fd);
    close(fd);

    StatusCode status = open_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
	assert_in_range(fd, 0, MAX_FD); // it will have a fd because of the previous create_db_file

    close(fd);
    unlink(test_file);
}

static void test_open_db_file_not_found(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "nonexistent_file.db";

    unlink(test_file);

    StatusCode status = open_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_FILE_OPEN_ERROR);
	assert_int_equal(fd, -1);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_db_file_success),
        cmocka_unit_test(test_create_db_file_exists),
        cmocka_unit_test(test_open_db_file_success),
        cmocka_unit_test(test_open_db_file_not_found),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}

