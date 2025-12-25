#include "parser.h"

#include <stdlib.h>

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
}

ParserNode *parser_parse(Parser *parser)
{
    ParserNode *node = malloc(sizeof(*node));

    if (!node) {
        parser->error_message = "parser: out of memory";
        return NULL;
    }

    Token token = parser_next(parser);
    while (token.type != TOKEN_EOF && token.type != TOKEN_INVALID) {
        token = parser_next(parser);
    }

    if (token.type == TOKEN_INVALID) {
        parser->error_message = "parser: invalid token";
        parser_node_init(node, PARSER_NODE_INVALID, token);
        return node;
    }

    parser_node_init(node, PARSER_NODE_TRANSLATION_UNIT, token);
    return node;
}

void parser_free_node(ParserNode *node)
{
    free(node);
}

const char *parser_error(const Parser *parser)
{
    return parser->error_message;
}
