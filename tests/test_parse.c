#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "parse.h"

static void test_create_db_header_success(void **state)
{
    (void)state;

    DBContext ctx = {0};
    int fd = open("test_db_file.db", O_CREAT | O_RDWR, 0644);
    assert_int_not_equal(fd, -1);

    ctx.db_fd = fd;

    StatusCode status = create_db_header(&ctx);
    assert_int_equal(status, STATUS_OK);
    assert_non_null(ctx.header);

    cleanup_context(&ctx);
    unlink("test_db_file.db");
}

static void test_create_db_header_null_context(void **state)
{
    (void)state;

    StatusCode status = create_db_header(NULL);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);
}

static void test_validate_db_header_success(void **state)
{
    (void)state;

    DBContext ctx = {0};
    int fd = open("test_db_file.db", O_CREAT | O_RDWR, 0644);
    assert_int_not_equal(fd, -1);

    ctx.db_fd = fd;
    create_db_header(&ctx);
    output_file(&ctx);

    StatusCode status = validate_db_header(&ctx);
    assert_int_equal(status, STATUS_OK);
    assert_non_null(ctx.header);

    cleanup_context(&ctx);
    unlink("test_db_file.db");
}

static void test_validate_db_header_corrupted(void **state)
{
    (void)state;

    DBContext ctx = {0};
    int fd = open("test_db_file.db", O_CREAT | O_RDWR, 0644);
    assert_int_not_equal(fd, -1);

    ctx.db_fd = fd;
    create_db_header(&ctx);

    ctx.header->magic = 0; // Corrupt the magic number
    lseek(fd, 0, SEEK_SET);
    write(fd, ctx.header, sizeof(DBHeader));

    StatusCode status = validate_db_header(&ctx);
    assert_int_equal(status, STATUS_DB_CORRUPTED);

    cleanup_context(&ctx);
    unlink("test_db_file.db");
}

static void test_add_employee_success(void **state)
{
    (void)state;

    DBContext ctx = {
        .db_fd = -1,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St,40";
    StatusCode status = add_employee(&ctx, add_string);
    assert_int_equal(status, STATUS_OK);

    assert_non_null(ctx.employees);
    assert_int_equal(ctx.header->count, 1);
    assert_string_equal(ctx.employees[0].name, "John Doe");
    assert_string_equal(ctx.employees[0].address, "123 Main St");
    assert_int_equal(ctx.employees[0].hours, 40);

    cleanup_context(&ctx);
}

static void test_add_employee_malformed_input(void **state)
{
    (void)state;

    DBContext ctx = {0};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St"; // Missing hours
    StatusCode status = add_employee(&ctx, add_string);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);

    cleanup_context(&ctx);
}

static void test_del_employee_by_name_found(void **state)
{
    (void)state;

    DBContext ctx = {
        .db_fd = -1,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St,40";
    add_employee(&ctx, add_string);

    StatusCode status = del_employee_by_name(&ctx, "John Doe");
    assert_int_equal(status, STATUS_OK);
    assert_int_equal(ctx.header->count, 0);

    cleanup_context(&ctx);
}

static void test_del_employee_by_name_not_found(void **state)
{
    (void)state;

    DBContext ctx = {
        .db_fd = -1,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St,40";
    add_employee(&ctx, add_string);

    StatusCode status = del_employee_by_name(&ctx, "Jane Doe");
    assert_int_equal(status, STATUS_EMPLOYEE_NOT_FOUND);

    cleanup_context(&ctx);
}

static void test_update_employee_hours_success(void **state)
{
    (void)state;

    DBContext ctx = {
        .db_fd = -1,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St,40";
    add_employee(&ctx, add_string);

    StatusCode status = update_employee_hours_by_name(&ctx, "John Doe", 50);
    assert_int_equal(status, STATUS_OK);
    assert_int_equal(ctx.employees[0].hours, 50);

    cleanup_context(&ctx);
}

static void test_update_employee_hours_not_found(void **state)
{
    (void)state;

    DBContext ctx = {
        .db_fd = -1,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))};
    create_db_header(&ctx);

    char add_string[] = "John Doe,123 Main St,40";
    add_employee(&ctx, add_string);

    StatusCode status = update_employee_hours_by_name(&ctx, "Jane Doe", 50);
    assert_int_equal(status, STATUS_EMPLOYEE_NOT_FOUND);

    cleanup_context(&ctx);
}

int main(void)
{
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_create_db_header_success),
        cmocka_unit_test(test_create_db_header_null_context),
        cmocka_unit_test(test_validate_db_header_success),
        cmocka_unit_test(test_validate_db_header_corrupted),
        cmocka_unit_test(test_add_employee_success),
        cmocka_unit_test(test_add_employee_malformed_input),
        cmocka_unit_test(test_del_employee_by_name_found),
        cmocka_unit_test(test_del_employee_by_name_not_found),
        cmocka_unit_test(test_update_employee_hours_success),
        cmocka_unit_test(test_update_employee_hours_not_found),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
