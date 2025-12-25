#include "checker.h"
#include "test_util.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static const char *current_test = "unknown";
static int total_passed = 0;
static int total_tests = 0;

static void begin_test(const char *name)
{
    current_test = name;
    total_tests++;
}

static void failf(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "test failure (%s): ", current_test);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(check_translation_unit, "check translation unit") \
    X(check_function_control_flow, "check function control flow") \
    X(check_function_call, "check function call") \
    X(check_invalid_token, "check invalid token") \
    X(check_missing_semicolon, "check missing semicolon") \
    X(check_expected_number, "check expected number")

static int error_contains(const char *error, const char *text)
{
    if (!error) {
        return 0;
    }

    return strstr(error, text) != NULL;
}

TEST(check_translation_unit, "check translation unit")
{
    Checker checker;

    checker_init(&checker, "int main; int value = 7;");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_function_control_flow, "check function control flow")
{
    Checker checker;

    checker_init(&checker, "int main(){while(0);if(1){return 2;}else{return 3;}}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_function_call, "check function call")
{
    Checker checker;

    checker_init(&checker, "int foo(){return 1;} int main(){return foo();}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_invalid_token, "check invalid token")
{
    Checker checker;

    checker_init(&checker, "@");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(error_contains(checker_error(&checker), "invalid"),
        "expected invalid token error");

    return 1;
}

TEST(check_missing_semicolon, "check missing semicolon")
{
    Checker checker;

    checker_init(&checker, "int value");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(error_contains(checker_error(&checker), "expected ';'"),
        "expected missing semicolon error");

    return 1;
}

TEST(check_expected_number, "check expected number")
{
    Checker checker;

    checker_init(&checker, "int value = ;");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(error_contains(checker_error(&checker), "expected expression"),
        "expected expression error");

    return 1;
}

typedef struct {
    const char *name;
    int (*fn)(void);
} TestCase;

#define TEST_ENTRY(name, description) { description, test_##name },

static const TestCase tests[] = {
    TEST_LIST(TEST_ENTRY)
};

int main(void)
{
    size_t i;

    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        begin_test(tests[i].name);
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
