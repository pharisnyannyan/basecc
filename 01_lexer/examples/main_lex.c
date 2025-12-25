#include "lexer.h"

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
    case TOKEN_SWITCH:
        return "TOKEN_SWITCH";
    case TOKEN_CASE:
        return "TOKEN_CASE";
    case TOKEN_INT:
        return "TOKEN_INT";
    case TOKEN_CHAR:
        return "TOKEN_CHAR";
    case TOKEN_SHORT:
        return "TOKEN_SHORT";
    case TOKEN_INVALID:
        return "TOKEN_INVALID";
    default:
        return "TOKEN_UNKNOWN";
    }
}

static void print_token(const Token *token)
{
    printf("type=%s length=%zu value=%ld text='%.*s'\n",
        token_type_name(token->type),
        token->length,
        token->value,
        (int)token->length,
        token->start);
}

int main(void)
{
    const char *source = "int main() { return 42; }";
    Lexer lexer;

    lexer_init(&lexer, source);

    for (;;) {
        Token token = lexer_next(&lexer);
        print_token(&token);
        if (token.type == TOKEN_EOF) {
            break;
        }
    }

    return 0;
}
