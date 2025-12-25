#include "test_util.h"

#include <stdarg.h>
#include <stdio.h>

static const char *current_test = "unknown";
static int total_passed = 0;
static int total_tests = 0;

void test_begin(const char *name)
{
    current_test = name;
    total_tests++;
}

void test_failf(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "test failure (%s): ", current_test);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

int test_error_contains(const char *error, const char *text)
{
    if (!error) {
        return 0;
    }

    return strstr(error, text) != NULL;
}

int test_run(const TestCase *tests, size_t count)
{
    size_t i;

    for (i = 0; i < count; i++) {
        test_begin(tests[i].name);
        if (tests[i].fn()) {
            total_passed++;
            printf("PASS: %s\n", current_test);
        } else {
            printf("FAIL: %s\n", current_test);
        }
    }

    printf("\nSummary: %d passed, %d failed, %d total\n",
        total_passed,
        total_tests - total_passed,
        total_tests);

    return total_tests == total_passed ? 0 : 1;
}
