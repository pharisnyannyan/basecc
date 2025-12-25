#ifndef BASECC_TEST_UTIL_H
#define BASECC_TEST_UTIL_H

#include <string.h>

#define ASSERT_TRUE(expr, message) \
    do { \
        if (!(expr)) { \
            failf("%s", message); \
            return 0; \
        } \
    } while (0)

#define ASSERT_TRUEF(expr, fmt, ...) \
    do { \
        if (!(expr)) { \
            failf(fmt, __VA_ARGS__); \
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
