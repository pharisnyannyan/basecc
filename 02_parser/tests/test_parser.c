#include "parser.h"
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
    X(parse_translation_unit, "parse translation unit") \
    X(parse_invalid_token, "parse invalid token")

TEST(parse_translation_unit, "parse translation unit")
{
    Parser parser;

    parser_init(&parser, "int main() { return 0; }");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit node");

    parser_free_node(node);
    return 1;
}

TEST(parse_invalid_token, "parse invalid token")
{
    Parser parser;

    parser_init(&parser, "@");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) != NULL, "expected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_INVALID,
        "expected invalid node");

    parser_free_node(node);
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
