#ifndef BASECC_PARSER_H
#define BASECC_PARSER_H

#include "lexer.h"

typedef enum ParserNodeType {
    PARSER_NODE_TRANSLATION_UNIT = 0,
    PARSER_NODE_INVALID
} ParserNodeType;

typedef struct ParserNode {
    ParserNodeType type;
    Token token;
} ParserNode;

typedef struct Parser {
    Lexer lexer;
    const char *error_message;
    Token last_token;
} Parser;

void parser_init(Parser *parser, const char *input);
Token parser_next(Parser *parser);
ParserNode *parser_parse(Parser *parser);
void parser_free_node(ParserNode *node);
const char *parser_error(const Parser *parser);
void parser_node_init(ParserNode *node, ParserNodeType type, Token token);

#endif
