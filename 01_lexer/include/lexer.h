#ifndef BASECC_LEXER_H
#define BASECC_LEXER_H

#include <stddef.h>

typedef enum TokenType {
    TOKEN_EOF = 0,
    TOKEN_IDENT,
    TOKEN_NUMBER,
    TOKEN_PUNCT,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_WHILE,
    TOKEN_SWITCH,
    TOKEN_CASE,
    TOKEN_INT,
    TOKEN_INVALID = 1000 /* keep stable value for compatibility */
} TokenType;

typedef struct Token {
    TokenType type;
    const char *start;
    size_t length;
    long value;
} Token;

typedef struct Lexer {
    const char *input;
    size_t pos;
} Lexer;

void lexer_init(Lexer *lexer, const char *input);
Token lexer_next(Lexer *lexer);

#endif
