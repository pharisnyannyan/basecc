#ifndef BASECC_PARSER_H
#define BASECC_PARSER_H

#include "lexer.h"

typedef enum ParserNodeType {
    PARSER_NODE_TRANSLATION_UNIT = 0,
    PARSER_NODE_DECLARATION,
    PARSER_NODE_FUNCTION,
    PARSER_NODE_BLOCK,
    PARSER_NODE_IF,
    PARSER_NODE_WHILE,
    PARSER_NODE_FOR,
    PARSER_NODE_RETURN,
    PARSER_NODE_BREAK,
    PARSER_NODE_CONTINUE,
    PARSER_NODE_EMPTY,
    PARSER_NODE_STRUCT,
    PARSER_NODE_TYPEDEF,
    PARSER_NODE_ASSIGN,
    PARSER_NODE_CALL,
    PARSER_NODE_IDENTIFIER,
    PARSER_NODE_MEMBER,
    PARSER_NODE_BINARY,
    PARSER_NODE_UNARY,
    PARSER_NODE_CAST,
    PARSER_NODE_SIZEOF,
    PARSER_NODE_NUMBER,
    PARSER_NODE_INVALID
} ParserNodeType;

typedef struct ParserNode ParserNode;

struct ParserNode {
    ParserNodeType type;
    Token token;
    Token type_token;
    int pointer_depth;
    int is_const;
    ParserNode *first_child;
    ParserNode *next;
};

typedef struct TypedefEntry TypedefEntry;

typedef struct Parser {
    Lexer lexer;
    const char *error_message;
    Token last_token;
    TypedefEntry *typedefs;
    size_t typedef_count;
    size_t typedef_capacity;
    int scope_depth;
} Parser;

void parser_init(Parser *parser, const char *input);
Token parser_next(Parser *parser);
ParserNode *parser_parse(Parser *parser);
void parser_free_node(ParserNode *node);
const char *parser_error(const Parser *parser);
void parser_node_init(ParserNode *node, ParserNodeType type, Token token);

#endif
