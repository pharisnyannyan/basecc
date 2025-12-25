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

static ParserNode *parser_parse_declaration(Parser *parser)
{
    Token token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (token.type != TOKEN_INT) {
        return parser_make_error(parser, token, "parser: expected 'int'");
    }

    parser_next(parser);
    token = parser->last_token;

    if (token.type == TOKEN_INVALID) {
        return parser_make_error(parser, token, "parser: invalid token");
    }

    if (token.type != TOKEN_IDENT) {
        return parser_make_error(parser, token, "parser: expected identifier");
    }

    parser_next(parser);

    ParserNode *node = parser_alloc_node(parser, PARSER_NODE_DECLARATION, token);
    if (!node) {
        return NULL;
    }

    if (parser_match_punct(parser, "=")) {
        ParserNode *init = parser_parse_number(parser);

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

        ParserNode *decl = parser_parse_declaration(parser);

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
