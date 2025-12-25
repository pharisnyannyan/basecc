#include "checker.h"
#include "test_util.h"


#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(check_translation_unit, "check translation unit") \
    X(check_function_control_flow, "check function control flow") \
    X(check_function_call, "check function call") \
    X(check_binary_expression, "check binary expression") \
    X(check_invalid_token, "check invalid token") \
    X(check_missing_semicolon, "check missing semicolon") \
    X(check_expected_number, "check expected number")

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

TEST(check_binary_expression, "check binary expression")
{
    Checker checker;

    checker_init(&checker, "int main(){return (8/2)+5%3 - 1;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_invalid_token, "check invalid token")
{
    Checker checker;

    checker_init(&checker, "@");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "invalid"),
        "expected invalid token error");

    return 1;
}

TEST(check_missing_semicolon, "check missing semicolon")
{
    Checker checker;

    checker_init(&checker, "int value");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "expected ';'"),
        "expected missing semicolon error");

    return 1;
}

TEST(check_expected_number, "check expected number")
{
    Checker checker;

    checker_init(&checker, "int value = ;");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "expected expression"),
        "expected expression error");

    return 1;
}

#define TEST_ENTRY(name, description) { description, test_##name },

static const TestCase tests[] = {
    TEST_LIST(TEST_ENTRY)
};

int main(void)
{
    return test_run(tests, sizeof(tests) / sizeof(tests[0]));
}
