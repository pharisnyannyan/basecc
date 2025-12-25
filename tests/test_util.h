#ifndef BASECC_TEST_UTIL_H
#define BASECC_TEST_UTIL_H

#include <stddef.h>
#include <string.h>

typedef int (*TestFn)(void);

typedef struct {
    const char *name;
    TestFn fn;
} TestCase;

void test_begin(const char *name);
void test_failf(const char *fmt, ...);
int test_run(const TestCase *tests, size_t count);
int test_error_contains(const char *error, const char *text);

#define failf test_failf

#define ASSERT_TRUE(expr, message) \
    do { \
        if (!(expr)) { \
            test_failf("%s", message); \
            return 0; \
        } \
    } while (0)

#define ASSERT_TRUEF(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            test_failf(fmt, __VA_ARGS__); \
            return 0; \
        } \
    } while (0)

#define ASSERT_TOKEN_TEXT(token_value, text_value) \
    do { \
        size_t length = strlen(text_value); \
        ASSERT_TRUE((token_value).length == length, "unexpected token length"); \
        ASSERT_TRUE(strncmp((token_value).start, (text_value), length) == 0, \
            "unexpected token text"); \
    } while (0)

#define ASSERT_TOKEN_VALUE(token_value, value_value) \
    do { \
        ASSERT_TRUE((token_value).type == TOKEN_NUMBER, \
            "expected TOKEN_NUMBER"); \
        ASSERT_TRUEF((token_value).value == (value_value), \
            "unexpected number value %ld", (token_value).value); \
    } while (0)

#endif
