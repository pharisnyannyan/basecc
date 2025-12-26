#include "parser.h"

#include <stdio.h>

static const char *token_type_name(TokenType type)
{
    switch (type) {
    case TOKEN_EOF:
        return "TOKEN_EOF";
    case TOKEN_IDENT:
        return "TOKEN_IDENT";
    case TOKEN_NUMBER:
        return "TOKEN_NUMBER";
    case TOKEN_PUNCT:
        return "TOKEN_PUNCT";
    case TOKEN_IF:
        return "TOKEN_IF";
    case TOKEN_ELSE:
        return "TOKEN_ELSE";
    case TOKEN_WHILE:
        return "TOKEN_WHILE";
    case TOKEN_FOR:
        return "TOKEN_FOR";
    case TOKEN_SWITCH:
        return "TOKEN_SWITCH";
    case TOKEN_CASE:
        return "TOKEN_CASE";
    case TOKEN_BREAK:
        return "TOKEN_BREAK";
    case TOKEN_CONTINUE:
        return "TOKEN_CONTINUE";
    case TOKEN_RETURN:
        return "TOKEN_RETURN";
    case TOKEN_CHAR:
        return "TOKEN_CHAR";
    case TOKEN_SHORT:
        return "TOKEN_SHORT";
    case TOKEN_INT:
        return "TOKEN_INT";
    case TOKEN_INVALID:
        return "TOKEN_INVALID";
    default:
        return "TOKEN_UNKNOWN";
    }
}

int main(void)
{
    const char *source = "int value = 7;";
    Parser parser;

    parser_init(&parser, source);

    for (;;) {
        Token token = parser_next(&parser);
        printf("type=%s length=%zu value=%ld text='%.*s'\n",
            token_type_name(token.type),
            token.length,
            token.value,
            (int)token.length,
            token.start);
        if (token.type == TOKEN_EOF) {
            break;
        }
    }

    return 0;
}
