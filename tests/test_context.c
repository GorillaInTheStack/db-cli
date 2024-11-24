#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "common.h"
#include "context.h"

static void test_cleanup_context_valid(void **state) {
    (void) state;

    DBContext ctx = {
        .db_fd = 10,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))
    };

    cleanup_context(&ctx);

    assert_null(ctx.header);
    assert_null(ctx.employees);
    assert_int_equal(ctx.db_fd, 0);
}

static void test_cleanup_context_null(void **state) {
    (void) state;

    DBContext ctx = {0};
    cleanup_context(&ctx);

    assert_null(ctx.header);
    assert_null(ctx.employees);
    assert_int_equal(ctx.db_fd, 0);
}

static void test_validate_context_valid(void **state) {
    (void) state;

    DBContext ctx = {
        .db_fd = 10,
        .header = malloc(sizeof(DBHeader)),
        .employees = malloc(2 * sizeof(Employee))
    };

    StatusCode status = validate_context(&ctx, true);
    assert_int_equal(status, STATUS_OK);

    cleanup_context(&ctx);
}

static void test_validate_context_no_header(void **state) {
    (void) state;

    DBContext ctx = {
        .db_fd = 10,
        .header = NULL,
        .employees = malloc(2 * sizeof(Employee))
    };

    StatusCode status = validate_context(&ctx, true);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);

    cleanup_context(&ctx);
}

static void test_validate_context_no_employees(void **state) {
    (void) state;

    DBContext ctx = {
        .db_fd = 10,
        .header = malloc(sizeof(DBHeader)),
        .employees = NULL
    };

    StatusCode status = validate_context(&ctx, true);
    assert_int_equal(status, STATUS_INVALID_ARGUMENT);

    cleanup_context(&ctx);
}

static void test_validate_context_no_employees_check_false(void **state) {
    (void) state;

    DBContext ctx = {
        .db_fd = 10,
        .header = malloc(sizeof(DBHeader)),
        .employees = NULL
    };

    StatusCode status = validate_context(&ctx, false);
    assert_int_equal(status, STATUS_OK);

    cleanup_context(&ctx);
}


static void test_convert_uint_to_network(void **state) {
    (void) state;

    unsigned int val = 0x12345678;
    unsigned int converted = convert_uint(val, true);
    assert_int_not_equal(val, converted);
    assert_int_equal(val, convert_uint(converted, false));
}

static void test_convert_ushort_to_network(void **state) {
    (void) state;

    unsigned short val = 0x1234;
    unsigned short converted = convert_ushort(val, true);
    assert_int_not_equal(val, converted);
    assert_int_equal(val, convert_ushort(converted, false));
}

int main(void) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_cleanup_context_valid),
        cmocka_unit_test(test_cleanup_context_null),
        cmocka_unit_test(test_validate_context_valid),
        cmocka_unit_test(test_validate_context_no_header),
        cmocka_unit_test(test_validate_context_no_employees),
        cmocka_unit_test(test_validate_context_no_employees_check_false),
        cmocka_unit_test(test_convert_uint_to_network),
        cmocka_unit_test(test_convert_ushort_to_network),
    };

    return cmocka_run_group_tests(tests, NULL, NULL);
}
