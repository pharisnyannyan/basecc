#include "checker.h"

#include <string.h>

static int checker_set_error(Checker *checker, const char *message)
{
    if (!checker->error_message) {
        checker->error_message = message;
    }

    return 0;
}

void checker_init(Checker *checker, const char *input)
{
    parser_init(&checker->parser, input);
    checker->error_message = NULL;
}

static int checker_validate_number(Checker *checker, const ParserNode *node)
{
    if (node->type != PARSER_NODE_NUMBER) {
        return checker_set_error(checker, "checker: expected number");
    }

    if (node->token.type != TOKEN_NUMBER) {
        return checker_set_error(checker, "checker: expected number token");
    }

    if (node->first_child) {
        return checker_set_error(checker,
            "checker: unexpected number children");
    }

    return 1;
}

static int token_is_punct(Token token, const char *text)
{
    size_t length = strlen(text);

    if (token.type != TOKEN_PUNCT) {
        return 0;
    }

    if (token.length != length) {
        return 0;
    }

    return strncmp(token.start, text, length) == 0;
}

static int checker_validate_binary_operator(Checker *checker, Token token)
{
    if (token_is_punct(token, "+")
        || token_is_punct(token, "-")
        || token_is_punct(token, "*")
        || token_is_punct(token, "/")
        || token_is_punct(token, "%")
        || token_is_punct(token, "&&")
        || token_is_punct(token, "||")) {
        return 1;
    }

    return checker_set_error(checker, "checker: expected binary operator");
}

static int checker_validate_unary_operator(Checker *checker, Token token)
{
    if (token_is_punct(token, "!")
        || token_is_punct(token, "+")
        || token_is_punct(token, "-")
        || token_is_punct(token, "*")
        || token_is_punct(token, "&")) {
        return 1;
    }

    return checker_set_error(checker, "checker: expected unary operator");
}

static int checker_validate_expression(Checker *checker,
    const ParserNode *node)
{
    const ParserNode *child = NULL;

    if (node->type == PARSER_NODE_NUMBER) {
        return checker_validate_number(checker, node);
    }

    if (node->type == PARSER_NODE_CALL) {
        if (node->token.type != TOKEN_IDENT) {
            return checker_set_error(checker,
                "checker: expected function identifier");
        }

        if (node->first_child) {
            return checker_set_error(checker,
                "checker: arguments not supported");
        }

        for (child = node->first_child; child; child = child->next) {
            if (!checker_validate_expression(checker, child)) {
                return 0;
            }
        }

        return 1;
    }

    if (node->type == PARSER_NODE_IDENTIFIER) {
        if (node->token.type != TOKEN_IDENT) {
            return checker_set_error(checker,
                "checker: expected identifier");
        }

        if (node->first_child) {
            return checker_set_error(checker,
                "checker: unexpected identifier children");
        }

        return 1;
    }

    if (node->type == PARSER_NODE_UNARY) {
        const ParserNode *operand = node->first_child;

        if (!operand || operand->next) {
            return checker_set_error(checker,
                "checker: expected unary operand");
        }

        if (!checker_validate_unary_operator(checker, node->token)) {
            return 0;
        }

        return checker_validate_expression(checker, operand);
    }

    if (node->type == PARSER_NODE_BINARY) {
        const ParserNode *left = node->first_child;
        const ParserNode *right = left ? left->next : NULL;

        if (!left || !right || right->next) {
            return checker_set_error(checker,
                "checker: expected binary operands");
        }

        if (!checker_validate_binary_operator(checker, node->token)) {
            return 0;
        }

        if (!checker_validate_expression(checker, left)) {
            return 0;
        }

        return checker_validate_expression(checker, right);
    }

    return checker_set_error(checker, "checker: expected expression");
}

static int checker_validate_statement(Checker *checker,
    const ParserNode *node)
{
    const ParserNode *child = NULL;

    switch (node->type) {
    case PARSER_NODE_BLOCK:
        for (child = node->first_child; child; child = child->next) {
            if (!checker_validate_statement(checker, child)) {
                return 0;
            }
        }
        return 1;
    case PARSER_NODE_IF: {
        const ParserNode *condition = node->first_child;
        const ParserNode *then_branch = condition ? condition->next : NULL;
        const ParserNode *else_branch = then_branch ? then_branch->next : NULL;

        if (!condition || !then_branch) {
            return checker_set_error(checker,
                "checker: incomplete if statement");
        }

        if (else_branch && else_branch->next) {
            return checker_set_error(checker,
                "checker: unexpected else statement");
        }

        if (!checker_validate_expression(checker, condition)) {
            return 0;
        }

        if (!checker_validate_statement(checker, then_branch)) {
            return 0;
        }

        if (else_branch && !checker_validate_statement(checker, else_branch)) {
            return 0;
        }

        return 1;
    }
    case PARSER_NODE_WHILE: {
        const ParserNode *condition = node->first_child;
        const ParserNode *body = condition ? condition->next : NULL;

        if (!condition || !body) {
            return checker_set_error(checker,
                "checker: incomplete while statement");
        }

        if (body->next) {
            return checker_set_error(checker,
                "checker: unexpected while statement");
        }

        if (!checker_validate_expression(checker, condition)) {
            return 0;
        }

        return checker_validate_statement(checker, body);
    }
    case PARSER_NODE_RETURN:
        if (!node->first_child || node->first_child->next) {
            return checker_set_error(checker,
                "checker: unexpected return statement");
        }

        return checker_validate_expression(checker, node->first_child);
    case PARSER_NODE_EMPTY:
        if (node->first_child) {
            return checker_set_error(checker,
                "checker: unexpected empty statement");
        }
        return 1;
    default:
        return checker_set_error(checker, "checker: expected statement");
    }
}

static int checker_validate_declaration(Checker *checker,
    const ParserNode *node)
{
    if (node->type != PARSER_NODE_DECLARATION) {
        return checker_set_error(checker, "checker: expected declaration");
    }

    if (node->token.type != TOKEN_IDENT) {
        return checker_set_error(checker, "checker: expected identifier");
    }

    if (node->first_child) {
        if (node->first_child->next) {
            return checker_set_error(checker,
                "checker: unexpected initializer list");
        }

        if (!checker_validate_expression(checker, node->first_child)) {
            return 0;
        }
    }

    return 1;
}

static int checker_validate_function(Checker *checker, const ParserNode *node)
{
    if (node->type != PARSER_NODE_FUNCTION) {
        return checker_set_error(checker, "checker: expected function");
    }

    if (node->token.type != TOKEN_IDENT) {
        return checker_set_error(checker, "checker: expected identifier");
    }

    if (!node->first_child || node->first_child->next) {
        return checker_set_error(checker, "checker: expected function body");
    }

    if (node->first_child->type != PARSER_NODE_BLOCK) {
        return checker_set_error(checker,
            "checker: expected function block");
    }

    return checker_validate_statement(checker, node->first_child);
}

static int checker_validate_translation_unit(Checker *checker,
    const ParserNode *node)
{
    const ParserNode *child = NULL;

    if (node->type != PARSER_NODE_TRANSLATION_UNIT) {
        return checker_set_error(checker,
            "checker: expected translation unit");
    }

    if (node->token.type != TOKEN_EOF) {
        return checker_set_error(checker, "checker: expected EOF token");
    }

    for (child = node->first_child; child; child = child->next) {
        if (child->type == PARSER_NODE_DECLARATION) {
            if (!checker_validate_declaration(checker, child)) {
                return 0;
            }
            continue;
        }

        if (child->type == PARSER_NODE_FUNCTION) {
            if (!checker_validate_function(checker, child)) {
                return 0;
            }
            continue;
        }

        return checker_set_error(checker,
            "checker: unexpected top-level node");
    }

    return 1;
}

int checker_check(Checker *checker)
{
    ParserNode *root = parser_parse(&checker->parser);
    const char *parser_message = parser_error(&checker->parser);

    if (!root) {
        return checker_set_error(checker, "checker: out of memory");
    }

    if (parser_message) {
        checker_set_error(checker, parser_message);
        parser_free_node(root);
        return 0;
    }

    if (!checker_validate_translation_unit(checker, root)) {
        parser_free_node(root);
        return 0;
    }

    parser_free_node(root);
    return 1;
}

const char *checker_error(const Checker *checker)
{
    return checker->error_message;
}
