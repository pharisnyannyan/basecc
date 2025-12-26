#include "parser.h"
#include "test_util.h"

#include <string.h>

#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(parse_translation_unit, "parse translation unit") \
    X(parse_type_declarations, "parse type declarations") \
    X(parse_pointer_declaration, "parse pointer declaration") \
    X(parse_function_control_flow, "parse function control flow") \
    X(parse_for_loop, "parse for loop") \
    X(parse_loop_control, "parse loop control") \
    X(parse_function_call, "parse function call") \
    X(parse_assignment_statement, "parse assignment statement") \
    X(parse_dereference_assignment, "parse dereference assignment") \
    X(parse_binary_expression, "parse binary expression") \
    X(parse_parenthesized_arithmetic, "parse parenthesized arithmetic") \
    X(parse_nested_parentheses, "parse nested parentheses") \
    X(parse_unary_arithmetic, "parse unary arithmetic") \
    X(parse_logical_expression, "parse logical expression") \
    X(parse_invalid_token, "parse invalid token") \
    X(parse_mismatched_parentheses, "parse mismatched parentheses") \
    X(parse_unexpected_closing_paren, "parse unexpected closing paren") \
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

TEST(parse_type_declarations, "parse type declarations")
{
    Parser parser;

    parser_init(&parser, "short main; char value = 7;");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit node");
    ASSERT_TRUE(node->first_child != NULL, "expected first declaration");
    ASSERT_TRUE(token_equals(node->first_child->token, "main"),
        "expected declaration name 'main'");
    ASSERT_TRUE(node->first_child->type_token.type == TOKEN_SHORT,
        "expected short type token");
    ASSERT_TRUE(node->first_child->next != NULL, "expected second declaration");
    ASSERT_TRUE(token_equals(node->first_child->next->token, "value"),
        "expected declaration name 'value'");
    ASSERT_TRUE(node->first_child->next->type_token.type == TOKEN_CHAR,
        "expected char type token");
    ASSERT_TRUE(node->first_child->next->first_child != NULL,
        "expected initializer");
    ASSERT_TRUE(node->first_child->next->first_child->token.value == 7,
        "expected initializer value 7");

    parser_free_node(node);
    return 1;
}

TEST(parse_pointer_declaration, "parse pointer declaration")
{
    Parser parser;
    ParserNode *expr = NULL;

    parser_init(&parser, "int value; int *ptr = &value; int main(){return *ptr;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit node");
    ASSERT_TRUE(node->first_child != NULL, "expected first declaration");
    ASSERT_TRUE(node->first_child->next != NULL, "expected second declaration");
    ASSERT_TRUE(node->first_child->next->type == PARSER_NODE_DECLARATION,
        "expected pointer declaration");
    ASSERT_TRUE(node->first_child->next->pointer_depth == 1,
        "expected pointer depth");
    ASSERT_TRUE(node->first_child->next->first_child != NULL,
        "expected pointer initializer");
    ASSERT_TRUE(node->first_child->next->first_child->type
        == PARSER_NODE_UNARY,
        "expected unary initializer");
    ASSERT_TRUE(token_equals(node->first_child->next->first_child->token, "&"),
        "expected address-of initializer");

    expr = node->first_child->next->next
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_UNARY,
        "expected dereference expression");
    ASSERT_TRUE(token_equals(expr->token, "*"),
        "expected dereference operator");
    ASSERT_TRUE(expr->first_child != NULL, "expected dereference operand");
    ASSERT_TRUE(expr->first_child->type == PARSER_NODE_IDENTIFIER,
        "expected identifier operand");

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

TEST(parse_for_loop, "parse for loop")
{
    Parser parser;

    parser_init(&parser,
        "int main(){int sum=0;for(int i=3;i;i=i - 1){sum=sum+i;}return sum;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit");
    ASSERT_TRUE(node->first_child != NULL, "expected function");
    ASSERT_TRUE(node->first_child->type == PARSER_NODE_FUNCTION,
        "expected function node");
    ASSERT_TRUE(node->first_child->first_child != NULL,
        "expected function body");
    ASSERT_TRUE(node->first_child->first_child->type == PARSER_NODE_BLOCK,
        "expected function block");
    ASSERT_TRUE(node->first_child->first_child->first_child != NULL,
        "expected first statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->next != NULL,
        "expected for statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->next->type
        == PARSER_NODE_FOR,
        "expected for statement");

    parser_free_node(node);
    return 1;
}

TEST(parse_loop_control, "parse loop control")
{
    Parser parser;

    parser_init(&parser,
        "int main(){while(1){break;}for(;;){continue;}return 0;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->type == PARSER_NODE_TRANSLATION_UNIT,
        "expected translation unit");
    ASSERT_TRUE(node->first_child != NULL, "expected function");
    ASSERT_TRUE(node->first_child->type == PARSER_NODE_FUNCTION,
        "expected function node");
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
        "expected for statement");
    ASSERT_TRUE(node->first_child->first_child->first_child->next->type
        == PARSER_NODE_FOR,
        "expected for statement");

    {
        ParserNode *while_stmt = node->first_child->first_child->first_child;
        ParserNode *while_body = while_stmt->first_child;
        while_body = while_body ? while_body->next : NULL;
        ASSERT_TRUE(while_body != NULL, "expected while body");
        ASSERT_TRUE(while_body->type == PARSER_NODE_BLOCK,
            "expected while body block");
        ASSERT_TRUE(while_body->first_child != NULL,
            "expected break statement");
        ASSERT_TRUE(while_body->first_child->type == PARSER_NODE_BREAK,
            "expected break statement");
    }

    {
        ParserNode *for_stmt = node->first_child->first_child->first_child->next;
        ParserNode *for_body = for_stmt->first_child;
        for_body = for_body ? for_body->next : NULL;
        for_body = for_body ? for_body->next : NULL;
        for_body = for_body ? for_body->next : NULL;
        ASSERT_TRUE(for_body != NULL, "expected for body");
        ASSERT_TRUE(for_body->type == PARSER_NODE_BLOCK,
            "expected for body block");
        ASSERT_TRUE(for_body->first_child != NULL,
            "expected continue statement");
        ASSERT_TRUE(for_body->first_child->type == PARSER_NODE_CONTINUE,
            "expected continue statement");
    }

    parser_free_node(node);
    return 1;
}

TEST(parse_function_call, "parse function call")
{
    Parser parser;

    parser_init(&parser,
        "int foo(int a, int b){return a + b;} int main(){return foo(1, 2);}");

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
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        ->first_child != NULL,
        "expected call arguments");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        ->first_child->type == PARSER_NODE_NUMBER,
        "expected first argument");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        ->first_child->next != NULL,
        "expected second argument");
    ASSERT_TRUE(node->first_child->next->first_child->first_child->first_child
        ->first_child->next->type == PARSER_NODE_NUMBER,
        "expected second argument");

    parser_free_node(node);
    return 1;
}

TEST(parse_assignment_statement, "parse assignment statement")
{
    Parser parser;

    parser_init(&parser, "int main(){int a=0; a = a + 1; return a;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->first_child != NULL, "expected function");
    ASSERT_TRUE(node->first_child->first_child != NULL,
        "expected function body");

    ParserNode *stmt = node->first_child->first_child->first_child;
    ASSERT_TRUE(stmt != NULL, "expected declaration statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_DECLARATION,
        "expected declaration statement");

    stmt = stmt->next;
    ASSERT_TRUE(stmt != NULL, "expected assignment statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_ASSIGN,
        "expected assignment statement");
    ASSERT_TRUE(stmt->first_child != NULL,
        "expected assignment target");
    ASSERT_TRUE(stmt->first_child->type == PARSER_NODE_IDENTIFIER,
        "expected assignment identifier");
    ASSERT_TRUE(token_equals(stmt->first_child->token, "a"),
        "expected assignment target");
    ASSERT_TRUE(stmt->first_child->next != NULL,
        "expected assignment expression");

    stmt = stmt->next;
    ASSERT_TRUE(stmt != NULL, "expected return statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_RETURN,
        "expected return statement");

    parser_free_node(node);
    return 1;
}

TEST(parse_dereference_assignment, "parse dereference assignment")
{
    Parser parser;

    parser_init(&parser,
        "int main(){int value=1; int *ptr=&value; *ptr = 2; return value;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");
    ASSERT_TRUE(node->first_child != NULL, "expected function");
    ASSERT_TRUE(node->first_child->first_child != NULL,
        "expected function body");

    ParserNode *stmt = node->first_child->first_child->first_child;
    ASSERT_TRUE(stmt != NULL, "expected declaration statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_DECLARATION,
        "expected declaration statement");

    stmt = stmt->next;
    ASSERT_TRUE(stmt != NULL, "expected declaration statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_DECLARATION,
        "expected declaration statement");

    stmt = stmt->next;
    ASSERT_TRUE(stmt != NULL, "expected assignment statement");
    ASSERT_TRUE(stmt->type == PARSER_NODE_ASSIGN,
        "expected assignment statement");
    ASSERT_TRUE(stmt->first_child != NULL,
        "expected assignment target");
    ASSERT_TRUE(stmt->first_child->type == PARSER_NODE_UNARY,
        "expected unary assignment target");
    ASSERT_TRUE(token_equals(stmt->first_child->token, "*"),
        "expected dereference assignment target");
    ASSERT_TRUE(stmt->first_child->next != NULL,
        "expected assignment expression");

    parser_free_node(node);
    return 1;
}

TEST(parse_binary_expression, "parse binary expression")
{
    Parser parser;
    ParserNode *expr = NULL;
    ParserNode *left = NULL;
    ParserNode *right = NULL;

    parser_init(&parser, "int main(){return 1+2*3 - 4%5;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");

    expr = node->first_child
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_BINARY,
        "expected binary expression");
    ASSERT_TRUE(token_equals(expr->token, "-"),
        "expected '-' operator");

    left = expr->first_child;
    right = left ? left->next : NULL;
    ASSERT_TRUE(left != NULL, "expected left expression");
    ASSERT_TRUE(right != NULL, "expected right expression");

    ASSERT_TRUE(left->type == PARSER_NODE_BINARY, "expected '+' node");
    ASSERT_TRUE(token_equals(left->token, "+"), "expected '+' operator");
    ASSERT_TRUE(right->type == PARSER_NODE_BINARY, "expected '%' node");
    ASSERT_TRUE(token_equals(right->token, "%"), "expected '%' operator");

    ASSERT_TRUE(left->first_child != NULL, "expected left operand");
    ASSERT_TRUE(left->first_child->type == PARSER_NODE_NUMBER,
        "expected number operand");
    ASSERT_TRUE(left->first_child->next != NULL, "expected right operand");
    ASSERT_TRUE(left->first_child->next->type == PARSER_NODE_BINARY,
        "expected '*' operand");
    ASSERT_TRUE(token_equals(left->first_child->next->token, "*"),
        "expected '*' operator");

    ASSERT_TRUE(right->first_child != NULL, "expected '%' left operand");
    ASSERT_TRUE(right->first_child->type == PARSER_NODE_NUMBER,
        "expected number operand");
    ASSERT_TRUE(right->first_child->next != NULL, "expected '%' right operand");
    ASSERT_TRUE(right->first_child->next->type == PARSER_NODE_NUMBER,
        "expected number operand");

    parser_free_node(node);
    return 1;
}

TEST(parse_parenthesized_arithmetic, "parse parenthesized arithmetic")
{
    Parser parser;
    ParserNode *expr = NULL;
    ParserNode *left = NULL;
    ParserNode *right = NULL;

    parser_init(&parser,
        "int main(){return (1 + 2) * (3 - 4) / 5 + 6;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");

    expr = node->first_child
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_BINARY,
        "expected binary expression");
    ASSERT_TRUE(token_equals(expr->token, "+"),
        "expected '+' operator");

    left = expr->first_child;
    right = left ? left->next : NULL;
    ASSERT_TRUE(left != NULL, "expected left expression");
    ASSERT_TRUE(right != NULL, "expected right expression");

    ASSERT_TRUE(left->type == PARSER_NODE_BINARY,
        "expected '/' expression");
    ASSERT_TRUE(token_equals(left->token, "/"),
        "expected '/' operator");
    ASSERT_TRUE(right->type == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(right->token.value == 6, "expected number 6");

    ASSERT_TRUE(left->first_child != NULL, "expected '/' left operand");
    ASSERT_TRUE(left->first_child->type == PARSER_NODE_BINARY,
        "expected '*' expression");
    ASSERT_TRUE(left->first_child->next != NULL, "expected '/' right operand");
    ASSERT_TRUE(left->first_child->next->type == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(left->first_child->next->token.value == 5, "expected number 5");

    ASSERT_TRUE(left->first_child->first_child != NULL,
        "expected '*' left operand");
    ASSERT_TRUE(left->first_child->first_child->type == PARSER_NODE_BINARY,
        "expected '+' expression");
    ASSERT_TRUE(left->first_child->first_child->next != NULL,
        "expected '*' right operand");
    ASSERT_TRUE(left->first_child->first_child->next->type
        == PARSER_NODE_BINARY,
        "expected '-' expression");

    ASSERT_TRUE(token_equals(left->first_child->first_child->token, "+"),
        "expected '+' operator");
    ASSERT_TRUE(token_equals(left->first_child->first_child->next->token, "-"),
        "expected '-' operator");

    ASSERT_TRUE(left->first_child->first_child->first_child != NULL,
        "expected '+' left operand");
    ASSERT_TRUE(left->first_child->first_child->first_child->type
        == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(left->first_child->first_child->first_child->token.value == 1,
        "expected number 1");
    ASSERT_TRUE(left->first_child->first_child->first_child->next != NULL,
        "expected '+' right operand");
    ASSERT_TRUE(left->first_child->first_child->first_child->next->type
        == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(left->first_child->first_child->first_child->next->token.value
        == 2,
        "expected number 2");

    ASSERT_TRUE(left->first_child->first_child->next->first_child != NULL,
        "expected '-' left operand");
    ASSERT_TRUE(left->first_child->first_child->next->first_child->type
        == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(left->first_child->first_child->next->first_child->token.value
        == 3,
        "expected number 3");
    ASSERT_TRUE(left->first_child->first_child->next->first_child->next != NULL,
        "expected '-' right operand");
    ASSERT_TRUE(left->first_child->first_child->next->first_child->next->type
        == PARSER_NODE_NUMBER,
        "expected number expression");
    ASSERT_TRUE(left->first_child->first_child->next->first_child->next
        ->token.value
        == 4,
        "expected number 4");

    parser_free_node(node);
    return 1;
}

TEST(parse_nested_parentheses, "parse nested parentheses")
{
    Parser parser;
    ParserNode *expr = NULL;

    parser_init(&parser,
        "int main(){return ((1 + (2 * 3)) - (4 / (5 + 6)));}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");

    expr = node->first_child
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_BINARY,
        "expected binary expression");
    ASSERT_TRUE(token_equals(expr->token, "-"),
        "expected '-' operator");

    ASSERT_TRUE(expr->first_child != NULL, "expected left expression");
    ASSERT_TRUE(expr->first_child->type == PARSER_NODE_BINARY,
        "expected '+' expression");
    ASSERT_TRUE(expr->first_child->next != NULL, "expected right expression");
    ASSERT_TRUE(expr->first_child->next->type == PARSER_NODE_BINARY,
        "expected '/' expression");

    parser_free_node(node);
    return 1;
}

TEST(parse_unary_arithmetic, "parse unary arithmetic")
{
    Parser parser;
    ParserNode *expr = NULL;
    ParserNode *left = NULL;
    ParserNode *right = NULL;

    parser_init(&parser, "int main(){return -(1 + 2) + +3;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");

    expr = node->first_child
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_BINARY,
        "expected binary expression");
    ASSERT_TRUE(token_equals(expr->token, "+"),
        "expected '+' operator");

    left = expr->first_child;
    right = left ? left->next : NULL;
    ASSERT_TRUE(left != NULL, "expected left expression");
    ASSERT_TRUE(right != NULL, "expected right expression");

    ASSERT_TRUE(left->type == PARSER_NODE_UNARY,
        "expected unary expression");
    ASSERT_TRUE(token_equals(left->token, "-"),
        "expected '-' unary operator");
    ASSERT_TRUE(right->type == PARSER_NODE_UNARY,
        "expected unary expression");
    ASSERT_TRUE(token_equals(right->token, "+"),
        "expected '+' unary operator");

    ASSERT_TRUE(left->first_child != NULL, "expected unary operand");
    ASSERT_TRUE(left->first_child->type == PARSER_NODE_BINARY,
        "expected '+' expression");
    ASSERT_TRUE(right->first_child != NULL, "expected unary operand");
    ASSERT_TRUE(right->first_child->type == PARSER_NODE_NUMBER,
        "expected number expression");

    parser_free_node(node);
    return 1;
}

TEST(parse_logical_expression, "parse logical expression")
{
    Parser parser;
    ParserNode *expr = NULL;
    ParserNode *left = NULL;
    ParserNode *right = NULL;

    parser_init(&parser, "int main(){return !1 || 0 && 2 + 3;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) == NULL, "unexpected parser error");

    expr = node->first_child
        ->first_child
        ->first_child
        ->first_child;
    ASSERT_TRUE(expr != NULL, "expected return expression");
    ASSERT_TRUE(expr->type == PARSER_NODE_BINARY,
        "expected logical or expression");
    ASSERT_TRUE(token_equals(expr->token, "||"),
        "expected '||' operator");

    left = expr->first_child;
    right = left ? left->next : NULL;
    ASSERT_TRUE(left != NULL, "expected left expression");
    ASSERT_TRUE(right != NULL, "expected right expression");

    ASSERT_TRUE(left->type == PARSER_NODE_UNARY,
        "expected unary expression");
    ASSERT_TRUE(token_equals(left->token, "!"), "expected '!'");
    ASSERT_TRUE(left->first_child != NULL, "expected unary operand");
    ASSERT_TRUE(left->first_child->type == PARSER_NODE_NUMBER,
        "expected unary number");

    ASSERT_TRUE(right->type == PARSER_NODE_BINARY,
        "expected logical and expression");
    ASSERT_TRUE(token_equals(right->token, "&&"),
        "expected '&&' operator");
    ASSERT_TRUE(right->first_child != NULL, "expected right left operand");
    ASSERT_TRUE(right->first_child->type == PARSER_NODE_NUMBER,
        "expected right left number");
    ASSERT_TRUE(right->first_child->next != NULL,
        "expected right right operand");
    ASSERT_TRUE(right->first_child->next->type == PARSER_NODE_BINARY,
        "expected additive expression");
    ASSERT_TRUE(token_equals(right->first_child->next->token, "+"),
        "expected '+' operator");

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

TEST(parse_mismatched_parentheses, "parse mismatched parentheses")
{
    Parser parser;

    parser_init(&parser, "int main(){return (1 + 2;}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) != NULL, "expected parser error");
    ASSERT_TRUE(test_error_contains(parser_error(&parser), "expected ')'"),
        "expected missing ')' error");
    ASSERT_TRUE(node->type == PARSER_NODE_INVALID,
        "expected invalid node");

    parser_free_node(node);
    return 1;
}

TEST(parse_unexpected_closing_paren, "parse unexpected closing paren")
{
    Parser parser;

    parser_init(&parser, "int main(){return );}");

    ParserNode *node = parser_parse(&parser);
    ASSERT_TRUE(node != NULL, "expected parser node");
    ASSERT_TRUE(parser_error(&parser) != NULL, "expected parser error");
    ASSERT_TRUE(test_error_contains(parser_error(&parser), "unexpected ')'"),
        "expected unexpected ')' error");
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
