#include "checker.h"

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

        if (!checker_validate_number(checker, node->first_child)) {
            return 0;
        }
    }

    return 1;
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
        if (!checker_validate_declaration(checker, child)) {
            return 0;
        }
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
