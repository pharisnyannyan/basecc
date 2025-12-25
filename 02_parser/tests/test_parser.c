#include "parser.h"
#include "test_util.h"

#include <string.h>

#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(parse_translation_unit, "parse translation unit") \
    X(parse_function_control_flow, "parse function control flow") \
    X(parse_function_call, "parse function call") \
    X(parse_invalid_token, "parse invalid token") \
    X(parse_missing_semicolon, "parse missing semicolon") \
    X(parse_expected_number, "parse expected number")

static int token_equals(Token token, const char *text)
{
    size_t length = strlen(text);

    if (token.length != length) {
        return 0;
    }

    return strncmp(token.start, text, length) == 0;
}

TEST(parse_translation_unit, "parse translation unit")
{
    Parser parser;

    parser_init(&parser, "int main; int value = 7;");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit node");
    ASSERT_TRUE(node->first_child != NULL, "expected declaration");
    ASSERT_TRUE(node->first_child->type == PARSER_NODE_DECLARATION,
        "expected declaration node");
    ASSERT_TRUE(token_equals(node->first_child->token, "main"),
        "expected declaration name 'main'");
    ASSERT_TRUE(node->first_child->first_child == NULL,
        "expected no initializer");
    ASSERT_TRUE(node->first_child->next != NULL,
        "expected second declaration");
    ASSERT_TRUE(token_equals(node->first_child->next->token, "value"),
        "expected declaration name 'value'");
    ASSERT_TRUE(node->first_child->next->first_child != NULL,
        "expected initializer");
    ASSERT_TRUE(node->first_child->next->first_child->type
        == PARSER_NODE_NUMBER,
        "expected number initializer");
    ASSERT_TRUE(node->first_child->next->first_child->token.value == 7,
        "expected initializer value 7");

    parser_free_node(node);
    return 1;
}

TEST(parse_function_control_flow, "parse function control flow")
{
    Parser parser;

    parser_init(&parser, "int main(){while(0);if(1){return 2;}else{return 3;}}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit");
    ASSERT_TRUE(node->first_child != NULL, "expected function");
    ASSERT_TRUE(node->first_child->type == PARSER_NODE_FUNCTION,
        "expected function node");
    ASSERT_TRUE(token_equals(node->first_child->token, "main"),
        "expected function name 'main'");
    ASSERT_TRUE(node->first_child->first_child != NULL,
        "expected function body");
    ASSERT_TRUE(node->first_child->first_child->type == PARSER_NODE_BLOCK,
        "expected function block");
    ASSERT_TRUE(node->first_child->first_child->first_child != NULL,
        "expected first statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->type
        == PARSER_NODE_WHILE,
        "expected while statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->next != NULL,
        "expected second statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->next->type
        == PARSER_NODE_IF,
        "expected if statement");

    parser_free_node(node);
    return 1;
}

TEST(parse_function_call, "parse function call")
{
    Parser parser;

    parser_init(&parser, "int foo(){return 1;} int main(){return foo();}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit");
    ASSERT_TRUE(node->first_child != NULL, "expected first function");
    ASSERT_TRUE(node->first_child->next != NULL, "expected second function");
    ASSERT_TRUE(node->first_child->next->type == PARSER_NODE_FUNCTION,
        "expected function node");
    ASSERT_TRUE(token_equals(node->first_child->next->token, "main"),
        "expected function name 'main'");
    ASSERT_TRUE(node->first_child->next->first_child != NULL,
        "expected function body");
    ASSERT_TRUE(node->first_child->next->first_child->first_child != NULL,
        "expected return statement");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->type
        == PARSER_NODE_RETURN,
        "expected return statement");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        != NULL,
        "expected return expression");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        ->type == PARSER_NODE_CALL,
        "expected call expression");
    ASSERT_TRUE(token_equals(
        node->first_child->next->first_child->first_child->first_child->token,
        "foo"),
        "expected call to 'foo'");

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

TEST(parse_missing_semicolon, "parse missing semicolon")
{
    Parser parser;

    parser_init(&parser, "int value");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) != NULL, "expected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_INVALID,
        "expected invalid node");

    parser_free_node(node);
    return 1;
}

TEST(parse_expected_number, "parse expected number")
{
    Parser parser;

    parser_init(&parser, "int value = ;");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) != NULL, "expected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_INVALID,
        "expected invalid node");

    parser_free_node(node);
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
