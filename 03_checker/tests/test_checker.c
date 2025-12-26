#include "checker.h"
#include "test_util.h"


#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(check_translation_unit, "check translation unit") \
    X(check_type_declarations, "check type declarations") \
    X(check_pointer_support, "check pointer support") \
    X(check_array_support, "check array support") \
    X(check_typedef_const_cast, "check typedef const cast") \
    X(check_static_declarations, "check static declarations") \
    X(check_struct_definition, "check struct definition") \
    X(check_function_control_flow, "check function control flow") \
    X(check_for_loop, "check for loop") \
    X(check_loop_control, "check loop control") \
    X(check_function_call, "check function call") \
    X(check_extern_function_declaration, "check extern function declaration") \
    X(check_assignment_statement, "check assignment statement") \
    X(check_dereference_assignment, "check dereference assignment") \
    X(check_binary_expression, "check binary expression") \
    X(check_parenthesized_arithmetic, "check parenthesized arithmetic") \
    X(check_nested_parentheses, "check nested parentheses") \
    X(check_unary_arithmetic, "check unary arithmetic") \
    X(check_sizeof, "check sizeof") \
    X(check_logical_expression, "check logical expression") \
    X(check_break_outside_loop, "check break outside loop") \
    X(check_continue_outside_loop, "check continue outside loop") \
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

TEST(check_array_support, "check array support")
{
    Checker checker;

    checker_init(&checker,
        "int global[3];"
        "int main(){int local[2]; global[0]=1; local[0]=global[0];"
        "return local[0];}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_typedef_const_cast, "check typedef const cast")
{
    Checker checker;

    checker_init(&checker,
        "typedef const int *ConstIntPtr;"
        "int main(){int value=0; void *ptr=&value;"
        "ConstIntPtr c=(ConstIntPtr)ptr; return (int)ptr;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_static_declarations, "check static declarations")
{
    Checker checker;

    checker_init(&checker,
        "static int global;"
        "const static char *ptr;"
        "int main(){static short local=2; static int *p=&global; return 0;}");

    ASSERT_TRUE(checker_check(&checker), "expected check success");
    ASSERT_TRUE(checker_error(&checker) == NULL, "unexpected error message");

    return 1;
}

TEST(check_struct_definition, "check struct definition")
{
    Checker checker;

    checker_init(&checker,
        "struct Pair { int left; char right; }; struct Pair value;");

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

TEST(check_loop_control, "check loop control")
{
    Checker checker;

    checker_init(&checker,
        "int main(){while(1){break;}for(;;){continue;}return 0;}");

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

TEST(check_extern_function_declaration, "check extern function declaration")
{
    Checker checker;

    checker_init(&checker,
        "extern int write(int fd, const char *buf, int count);"
        "int main(){char buffer[4]; int written;"
        "written = write(1, buffer, 0); return written;}");

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

TEST(check_dereference_assignment, "check dereference assignment")
{
    Checker checker;

    checker_init(&checker,
        "int main(){int value=1; int *ptr=&value; *ptr = 2; return value;}");

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

TEST(check_sizeof, "check sizeof")
{
    Checker checker;

    checker_init(&checker,
        "int main(){int value; return sizeof(value) + sizeof(int);}");

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

TEST(check_break_outside_loop, "check break outside loop")
{
    Checker checker;

    checker_init(&checker, "int main(){break;}");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "break"),
        "expected break error");

    return 1;
}

TEST(check_continue_outside_loop, "check continue outside loop")
{
    Checker checker;

    checker_init(&checker, "int main(){continue;}");

    ASSERT_TRUE(!checker_check(&checker), "expected check failure");
    ASSERT_TRUE(test_error_contains(checker_error(&checker), "continue"),
        "expected continue error");

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
