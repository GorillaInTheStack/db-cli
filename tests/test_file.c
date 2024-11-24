#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdarg.h>
#include <linux/limits.h>
#include <limits.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>   
#include <errno.h>

#include "file.h"    
#include "utils.h"

static unsigned long max_fd = 0;

static int setup_max_fd(void **state) {
    (void) state;
    max_fd = get_max_fd();
    return 0;  
}

static void test_create_db_file_success(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    unlink(test_file);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    assert_in_range(fd, 0, max_fd);

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
    assert_in_range(fd, 0, max_fd);

    unlink(test_file);
}

static void test_create_db_file_path_too_long(void **state) {
    (void) state;

    int fd = -1;
    char long_path[PATH_MAX + 10];
    memset(long_path, 'a', sizeof(long_path));
    long_path[sizeof(long_path) - 1] = '\0';

    StatusCode status = create_db_file(long_path, &fd);
    assert_true(status < 0);
}

static void test_create_db_file_repeated_unlink(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    unlink(test_file);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    close(fd);

    assert_int_equal(unlink(test_file), 0);
    assert_int_equal(unlink(test_file), -1);  // File should no longer exist
}

static void test_create_db_file_path_traversal(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "../test_file.db";

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);
}

static void test_create_db_file_symlink(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";
    const char *symlink_file = "symlink.db";

    unlink(test_file);
    unlink(symlink_file);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    close(fd);

    symlink(test_file, symlink_file);

    status = create_db_file(symlink_file, &fd);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);  // Should detect and handle symlink correctly

    unlink(test_file);
    unlink(symlink_file);
}

static void test_create_db_file_no_write_permission(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    unlink(test_file);

    StatusCode status = create_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    close(fd);

    chmod(test_file, 0444);

    status = create_db_file(test_file, &fd);
    assert_true(status < 0);
    chmod(test_file, 0644);

    unlink(test_file);
}

static void test_create_db_file_arbitrary_overwrite(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "/etc/passwd";

    StatusCode status = create_db_file(test_file, &fd);
    assert_true(status < 0);
}

static void test_open_db_file_success(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";

    create_db_file(test_file, &fd);
    close(fd);

    StatusCode status = open_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    assert_in_range(fd, 0, max_fd);

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

static void test_open_db_file_path_too_long(void **state) {
    (void) state;

    int fd = -1;
    char long_path[PATH_MAX + 10];
    memset(long_path, 'a', sizeof(long_path));
    long_path[sizeof(long_path) - 1] = '\0';

    StatusCode status = open_db_file(long_path, &fd);
    assert_true(status < 0);
}

static void test_open_db_file_path_traversal(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "../test_file.db";

    StatusCode status = open_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);
}

static void test_open_db_file_large_file_descriptor(void **state) {
    (void) state;

    int fd = INT_MAX;

    close(fd);

    StatusCode status = open_db_file("test_file.db", &fd);
    assert_int_equal(status, STATUS_FILE_OPEN_ERROR);
}

static void test_open_db_file_symlink(void **state) {
    (void) state;

    int fd = -1;
    const char *test_file = "test_file.db";
    const char *symlink_file = "symlink.db";

    unlink(test_file);
    unlink(symlink_file);

    StatusCode status = open_db_file(test_file, &fd);
    assert_int_equal(status, STATUS_OK);
    close(fd);

    symlink(test_file, symlink_file);

    status = open_db_file(symlink_file, &fd);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);  // Should detect and handle symlink correctly

    unlink(test_file);
    unlink(symlink_file);
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_db_file_success),
        cmocka_unit_test(test_create_db_file_exists),
        cmocka_unit_test(test_create_db_file_path_too_long),
        cmocka_unit_test(test_create_db_file_repeated_unlink),
        cmocka_unit_test(test_create_db_file_no_write_permission),
        cmocka_unit_test(test_create_db_file_path_traversal),
        cmocka_unit_test(test_create_db_file_symlink),
        cmocka_unit_test(test_create_db_file_arbitrary_overwrite),

        cmocka_unit_test(test_open_db_file_success),
        cmocka_unit_test(test_open_db_file_not_found),
        cmocka_unit_test(test_open_db_file_path_too_long),
        cmocka_unit_test(test_open_db_file_path_traversal),
        cmocka_unit_test(test_open_db_file_large_file_descriptor),
    };

    return cmocka_run_group_tests(tests, setup_max_fd, NULL);
}
