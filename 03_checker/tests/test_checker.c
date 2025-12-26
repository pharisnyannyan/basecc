#include "checker.h"
#include "test_util.h"


#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(check_translation_unit, "check translation unit") \
    X(check_type_declarations, "check type declarations") \
    X(check_pointer_support, "check pointer support") \
    X(check_function_control_flow, "check function control flow") \
    X(check_for_loop, "check for loop") \
    X(check_function_call, "check function call") \
    X(check_assignment_statement, "check assignment statement") \
    X(check_binary_expression, "check binary expression") \
    X(check_parenthesized_arithmetic, "check parenthesized arithmetic") \
    X(check_nested_parentheses, "check nested parentheses") \
    X(check_unary_arithmetic, "check unary arithmetic") \
    X(check_logical_expression, "check logical expression") \
    X(check_invalid_token, "check invalid token") \
    X(check_mismatched_parentheses, "check mismatched parentheses") \
    X(check_unexpected_closing_paren, "check unexpected closing paren") \
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

TEST(check_type_declarations, "check type declarations")
{
    Checker checker;

    checker_init(&checker, "short main; char value = 7;");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_pointer_support, "check pointer support")
{
    Checker checker;

    checker_init(&checker, "int value; int *ptr = &value; int main(){return *ptr;}");

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

TEST(check_for_loop, "check for loop")
{
    Checker checker;

    checker_init(&checker,
        "int main(){int sum=0;for(int i=3;i;i=i - 1){sum=sum+i;}return sum;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_function_call, "check function call")
{
    Checker checker;

    checker_init(&checker,
        "int foo(int a, int b){return a + b;} int main(){return foo(1, 2);}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_assignment_statement, "check assignment statement")
{
    Checker checker;

    checker_init(&checker, "int main(){int a=0; a = a + 1; return a;}");

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

TEST(check_parenthesized_arithmetic, "check parenthesized arithmetic")
{
    Checker checker;

    checker_init(&checker,
        "int main(){return (1 + 2) * (3 - 4) / 5 + 6;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_nested_parentheses, "check nested parentheses")
{
    Checker checker;

    checker_init(&checker,
        "int main(){return ((1 + (2 * 3)) - (4 / (5 + 6)));}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_unary_arithmetic, "check unary arithmetic")
{
    Checker checker;

    checker_init(&checker, "int main(){return -(1 + 2) + +3;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_logical_expression, "check logical expression")
{
    Checker checker;

    checker_init(&checker, "int main(){return !1 || 0 && 2 + 3;}");

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

TEST(check_mismatched_parentheses, "check mismatched parentheses")
{
    Checker checker;

    checker_init(&checker, "int main(){return (1 + 2;}");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "expected ')'"),
        "expected missing ')' error");

    return 1;
}

TEST(check_unexpected_closing_paren, "check unexpected closing paren")
{
    Checker checker;

    checker_init(&checker, "int main(){return );}");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "unexpected ')'"),
        "expected unexpected ')' error");

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
