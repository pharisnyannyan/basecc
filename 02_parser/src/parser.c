#include "parser.h"

#include <stdlib.h>
#include <string.h>

void parser_init(Parser *parser, const char *input)
{
    lexer_init(&parser->lexer, input);
    parser->error_message = NULL;
    parser->last_token = lexer_next(&parser->lexer);
}

Token parser_next(Parser *parser)
{
    Token current = parser->last_token;

    if (current.type != TOKEN_EOF && current.type != TOKEN_INVALID) {
        parser->last_token = lexer_next(&parser->lexer);
    }

    return current;
}

void parser_node_init(ParserNode *node, ParserNodeType type, Token token)
{
    node->type = type;
    node->token = token;
    node->type_token = token;
    node->first_child = NULL;
    node->next = NULL;
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

static int token_is_type(Token token)
{
    return token.type == TOKEN_INT
        || token.type == TOKEN_SHORT
        || token.type == TOKEN_CHAR;
}

static ParserNode *parser_alloc_node(Parser *parser, ParserNodeType type,
    Token token)
{
    ParserNode *node = malloc(sizeof(*node));

    if (!node) {
        parser->error_message = "parser: out of memory";
        return NULL;
    }

    parser_node_init(node, type, token);
    return node;
}

static ParserNode *parser_make_error(Parser *parser, Token token,
    const char *message)
{
    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_INVALID, token);

    if (!node) {
        return NULL;
    }

    if (!parser->error_message) {
        parser->error_message = message;
    }

    return node;
}

static int parser_match_punct(Parser *parser, const char *text)
{
    Token token = parser->last_token;

    if (!token_is_punct(token, text)) {
        return 0;
    }

    parser_next(parser);
    return 1;
}

static ParserNode *parser_parse_number(Parser *parser)
{
    Token token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (token.type != TOKEN_NUMBER) {
        return parser_make_error(parser, token, "parser: expected number");
    }

    parser_next(parser);
    return parser_alloc_node(parser, PARSER_NODE_NUMBER, token);
}

static ParserNode *parser_parse_expression(Parser *parser);
static ParserNode *parser_parse_unary(Parser *parser);

static ParserNode *parser_parse_call(Parser *parser, Token name_token)
{
    ParserNode *node = NULL;
    ParserNode **tail = NULL;

    if (!parser_match_punct(parser, "(")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected '('");
    }

    node = parser_alloc_node(parser, PARSER_NODE_CALL, name_token);
    if (!node) {
        return NULL;
    }

    tail = &node->first_child;

    if (!token_is_punct(parser->last_token, ")")) {
        ParserNode *arg = parser_parse_expression(parser);
        if (!arg || arg->type == PARSER_NODE_INVALID) {
            parser_free_node(node);
            return arg;
        }

        *tail = arg;
        tail = &arg->next;

        while (parser_match_punct(parser, ",")) {
            arg = parser_parse_expression(parser);
            if (!arg || arg->type == PARSER_NODE_INVALID) {
                parser_free_node(node);
                return arg;
            }

            *tail = arg;
            tail = &arg->next;
        }
    }

    if (!parser_match_punct(parser, ")")) {
        ParserNode *error_node = parser_make_error(parser, parser->last_token,
            "parser: expected ')'");
        parser_free_node(node);
        return error_node;
    }

    return node;
}

static ParserNode *parser_parse_primary(Parser *parser)
{
    Token token = parser->last_token;

    if (token.type == TOKEN_NUMBER) {
        return parser_parse_number(parser);
    }

    if (token.type == TOKEN_IDENT) {
        parser_next(parser);
        return parser_parse_call(parser, token);
    }

    if (token_is_punct(token, "(")) {
        ParserNode *expr = NULL;

        parser_next(parser);
        expr = parser_parse_expression(parser);
        if (!expr || expr->type == PARSER_NODE_INVALID) {
            return expr;
        }

        if (!parser_match_punct(parser, ")")) {
            ParserNode *error_node = parser_make_error(parser,
                parser->last_token,
                "parser: expected ')'");
            parser_free_node(expr);
            return error_node;
        }

        return expr;
    }

    if (token_is_punct(token, ")")) {
        return parser_make_error(parser, token,
            "parser: unexpected ')'");
    }

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    return parser_make_error(parser, token, "parser: expected expression");
}

static ParserNode *parser_parse_unary(Parser *parser)
{
    Token token = parser->last_token;

    if (token_is_punct(token, "!")
        || token_is_punct(token, "+")
        || token_is_punct(token, "-")) {
        ParserNode *node = NULL;
        ParserNode *operand = NULL;

        parser_next(parser);
        operand = parser_parse_unary(parser);
        if (!operand || operand->type == PARSER_NODE_INVALID) {
            return operand;
        }

        node = parser_alloc_node(parser, PARSER_NODE_UNARY, token);
        if (!node) {
            parser_free_node(operand);
            return NULL;
        }

        node->first_child = operand;
        return node;
    }

    return parser_parse_primary(parser);
}

static ParserNode *parser_parse_multiplicative(Parser *parser)
{
    ParserNode *left = parser_parse_unary(parser);

    if (!left || left->type == PARSER_NODE_INVALID) {
        return left;
    }

    while (token_is_punct(parser->last_token, "*")
        || token_is_punct(parser->last_token, "/")
        || token_is_punct(parser->last_token, "%")) {
        Token op = parser->last_token;
        ParserNode *right = NULL;
        ParserNode *node = NULL;

        parser_next(parser);

        right = parser_parse_unary(parser);
        if (!right || right->type == PARSER_NODE_INVALID) {
            parser_free_node(left);
            return right;
        }

        node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
        if (!node) {
            parser_free_node(left);
            parser_free_node(right);
            return NULL;
        }

        node->first_child = left;
        left->next = right;
        left = node;
    }

    return left;
}

static ParserNode *parser_parse_additive(Parser *parser)
{
    ParserNode *left = parser_parse_multiplicative(parser);

    if (!left || left->type == PARSER_NODE_INVALID) {
        return left;
    }

    while (token_is_punct(parser->last_token, "+")
        || token_is_punct(parser->last_token, "-")) {
        Token op = parser->last_token;
        ParserNode *right = NULL;
        ParserNode *node = NULL;

        parser_next(parser);

        right = parser_parse_multiplicative(parser);
        if (!right || right->type == PARSER_NODE_INVALID) {
            parser_free_node(left);
            return right;
        }

        node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
        if (!node) {
            parser_free_node(left);
            parser_free_node(right);
            return NULL;
        }

        node->first_child = left;
        left->next = right;
        left = node;
    }

    return left;
}

static ParserNode *parser_parse_logical_and(Parser *parser)
{
    ParserNode *left = parser_parse_additive(parser);

    if (!left || left->type == PARSER_NODE_INVALID) {
        return left;
    }

    while (token_is_punct(parser->last_token, "&&")) {
        Token op = parser->last_token;
        ParserNode *right = NULL;
        ParserNode *node = NULL;

        parser_next(parser);

        right = parser_parse_additive(parser);
        if (!right || right->type == PARSER_NODE_INVALID) {
            parser_free_node(left);
            return right;
        }

        node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
        if (!node) {
            parser_free_node(left);
            parser_free_node(right);
            return NULL;
        }

        node->first_child = left;
        left->next = right;
        left = node;
    }

    return left;
}

static ParserNode *parser_parse_logical_or(Parser *parser)
{
    ParserNode *left = parser_parse_logical_and(parser);

    if (!left || left->type == PARSER_NODE_INVALID) {
        return left;
    }

    while (token_is_punct(parser->last_token, "||")) {
        Token op = parser->last_token;
        ParserNode *right = NULL;
        ParserNode *node = NULL;

        parser_next(parser);

        right = parser_parse_logical_and(parser);
        if (!right || right->type == PARSER_NODE_INVALID) {
            parser_free_node(left);
            return right;
        }

        node = parser_alloc_node(parser, PARSER_NODE_BINARY, op);
        if (!node) {
            parser_free_node(left);
            parser_free_node(right);
            return NULL;
        }

        node->first_child = left;
        left->next = right;
        left = node;
    }

    return left;
}

static ParserNode *parser_parse_expression(Parser *parser)
{
    return parser_parse_logical_or(parser);
}

static ParserNode *parser_parse_statement(Parser *parser);

static ParserNode *parser_parse_block(Parser *parser)
{
    Token token = parser->last_token;
    ParserNode *block = NULL;
    ParserNode **tail = NULL;

    if (!parser_match_punct(parser, "{")) {
        return parser_make_error(parser, token, "parser: expected '{'");
    }

    block = parser_alloc_node(parser, PARSER_NODE_BLOCK, token);
    if (!block) {
        return NULL;
    }

    tail = &block->first_child;

    while (!token_is_punct(parser->last_token, "}")) {
        if (parser->last_token.type == TOKEN_EOF) {
            ParserNode *error_node = parser_make_error(parser,
                parser->last_token,
                "parser: expected '}'");
            parser_free_node(block);
            return error_node;
        }

        if (parser->last_token.type == TOKEN_INVALID) {
            ParserNode *error_node = parser_make_error(parser,
                parser->last_token,
                "parser: invalid token");
            parser_free_node(block);
            return error_node;
        }

        ParserNode *stmt = parser_parse_statement(parser);
        if (!stmt || stmt->type == PARSER_NODE_INVALID) {
            parser_free_node(block);
            return stmt;
        }

        *tail = stmt;
        tail = &stmt->next;
    }

    parser_next(parser);
    return block;
}

static ParserNode *parser_parse_if(Parser *parser)
{
    Token token = parser->last_token;

    parser_next(parser);

    if (!parser_match_punct(parser, "(")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected '('");
    }

    ParserNode *condition = parser_parse_expression(parser);
    if (!condition || condition->type == PARSER_NODE_INVALID) {
        return condition;
    }

    if (!parser_match_punct(parser, ")")) {
        ParserNode *error_node = parser_make_error(parser, parser->last_token,
            "parser: expected ')'");
        parser_free_node(condition);
        return error_node;
    }

    ParserNode *then_branch = parser_parse_statement(parser);
    if (!then_branch || then_branch->type == PARSER_NODE_INVALID) {
        parser_free_node(condition);
        return then_branch;
    }

    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_IF, token);
    if (!node) {
        parser_free_node(condition);
        parser_free_node(then_branch);
        return NULL;
    }

    node->first_child = condition;
    condition->next = then_branch;

    if (parser->last_token.type == TOKEN_ELSE) {
        parser_next(parser);
        ParserNode *else_branch = parser_parse_statement(parser);
        if (!else_branch || else_branch->type == PARSER_NODE_INVALID) {
            parser_free_node(node);
            return else_branch;
        }
        then_branch->next = else_branch;
    }

    return node;
}

static ParserNode *parser_parse_while(Parser *parser)
{
    Token token = parser->last_token;

    parser_next(parser);

    if (!parser_match_punct(parser, "(")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected '('");
    }

    ParserNode *condition = parser_parse_expression(parser);
    if (!condition || condition->type == PARSER_NODE_INVALID) {
        return condition;
    }

    if (!parser_match_punct(parser, ")")) {
        ParserNode *error_node = parser_make_error(parser, parser->last_token,
            "parser: expected ')'");
        parser_free_node(condition);
        return error_node;
    }

    ParserNode *body = parser_parse_statement(parser);
    if (!body || body->type == PARSER_NODE_INVALID) {
        parser_free_node(condition);
        return body;
    }

    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_WHILE, token);
    if (!node) {
        parser_free_node(condition);
        parser_free_node(body);
        return NULL;
    }

    node->first_child = condition;
    condition->next = body;
    return node;
}

static ParserNode *parser_parse_return(Parser *parser)
{
    Token token = parser->last_token;

    parser_next(parser);

    ParserNode *expr = parser_parse_expression(parser);
    if (!expr || expr->type == PARSER_NODE_INVALID) {
        return expr;
    }

    if (!parser_match_punct(parser, ";")) {
        ParserNode *error_node = parser_make_error(parser, parser->last_token,
            "parser: expected ';'");
        parser_free_node(expr);
        return error_node;
    }

    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_RETURN, token);
    if (!node) {
        parser_free_node(expr);
        return NULL;
    }

    node->first_child = expr;
    return node;
}

static ParserNode *parser_parse_statement(Parser *parser)
{
    Token token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (token.type == TOKEN_IF) {
        return parser_parse_if(parser);
    }

    if (token.type == TOKEN_WHILE) {
        return parser_parse_while(parser);
    }

    if (token.type == TOKEN_RETURN) {
        return parser_parse_return(parser);
    }

    if (token_is_punct(token, "{")) {
        return parser_parse_block(parser);
    }

    if (token_is_punct(token, ";")) {
        parser_next(parser);
        return parser_alloc_node(parser, PARSER_NODE_EMPTY, token);
    }

    return parser_make_error(parser, token, "parser: expected statement");
}

static ParserNode *parser_parse_declaration(Parser *parser, Token name_token,
    Token type_token)
{
    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_DECLARATION,
        name_token);
    if (!node) {
        return NULL;
    }

    node->type_token = type_token;

    if (parser_match_punct(parser, "=")) {
        ParserNode *init = parser_parse_expression(parser);

        if (!init || init->type == PARSER_NODE_INVALID) {
            parser_free_node(node);
            return init;
        }

        node->first_child = init;
    }

    if (!parser_match_punct(parser, ";")) {
        Token error_token = parser->last_token;
        ParserNode *error_node = parser_make_error(parser, error_token,
            "parser: expected ';'");
        parser_free_node(node);
        return error_node;
    }

    return node;
}

static ParserNode *parser_parse_function(Parser *parser, Token name_token,
    Token type_token)
{
    if (!parser_match_punct(parser, "(")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected '('");
    }

    if (!parser_match_punct(parser, ")")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected ')'");
    }

    if (!token_is_punct(parser->last_token, "{")) {
        return parser_make_error(parser, parser->last_token,
            "parser: expected '{'");
    }

    ParserNode *body = parser_parse_block(parser);
    if (!body || body->type == PARSER_NODE_INVALID) {
        return body;
    }

    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_FUNCTION,
        name_token);
    if (!node) {
        parser_free_node(body);
        return NULL;
    }

    node->type_token = type_token;
    node->first_child = body;
    return node;
}

static ParserNode *parser_parse_external(Parser *parser)
{
    Token token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (!token_is_type(token)) {
        return parser_make_error(parser, token, "parser: expected type");
    }

    Token type_token = token;

    parser_next(parser);
    token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (token.type != TOKEN_IDENT) {
        return parser_make_error(parser, token, "parser: expected identifier");
    }

    parser_next(parser);

    if (token_is_punct(parser->last_token, "(")) {
        return parser_parse_function(parser, token, type_token);
    }

    return parser_parse_declaration(parser, token, type_token);
}

ParserNode *parser_parse(Parser *parser)
{
    ParserNode *root = parser_alloc_node(parser, PARSER_NODE_TRANSLATION_UNIT,
        parser->last_token);
    ParserNode **tail = NULL;

    if (!root) {
        return NULL;
    }

    tail = &root->first_child;

    while (parser->last_token.type != TOKEN_EOF) {
        if (parser->last_token.type == TOKEN_INVALID) {
            ParserNode *error_node = parser_make_error(parser,
                parser->last_token,
                "parser: invalid token");
            parser_free_node(root);
            return error_node;
        }

        ParserNode *decl = parser_parse_external(parser);

        if (!decl || decl->type == PARSER_NODE_INVALID) {
            parser_free_node(root);
            return decl;
        }

        *tail = decl;
        tail = &decl->next;
    }

    root->token = parser->last_token;
    return root;
}

void parser_free_node(ParserNode *node)
{
    if (!node) {
        return;
    }

    parser_free_node(node->first_child);
    parser_free_node(node->next);
    free(node);
}

const char *parser_error(const Parser *parser)
{
    return parser->error_message;
}
