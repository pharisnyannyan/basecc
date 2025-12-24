#include "lexer.h"

#include <ctype.h>
#include <string.h>

static int is_ident_start(int ch)
{
    return isalpha(ch) || ch == '_';
}

static int is_ident_continue(int ch)
{
    return isalnum(ch) || ch == '_';
}

static Token make_token(TokenType type, const char *start, size_t length)
{
    Token token;

    token.type = type;
    token.start = start;
    token.length = length;
    token.value = 0;
    return token;
}

void lexer_init(Lexer *lexer, const char *input)
{
    lexer->input = input;
    lexer->pos = 0;
}

static void skip_whitespace(Lexer *lexer)
{
    while (lexer->input[lexer->pos] != '\0') {
        unsigned char ch = (unsigned char)lexer->input[lexer->pos];

        if (!isspace(ch)) {
            return;
        }
        lexer->pos++;
    }
}

static Token lex_number(Lexer *lexer)
{
    size_t start = lexer->pos;
    long value = 0;

    while (isdigit((unsigned char)lexer->input[lexer->pos])) {
        value = (value * 10) + (lexer->input[lexer->pos] - '0');
        lexer->pos++;
    }

    Token token = make_token(TOKEN_NUMBER, lexer->input + start,
        lexer->pos - start);
    token.value = value;
    return token;
}

static Token lex_negative_number(Lexer *lexer)
{
    size_t start = lexer->pos;
    long value = 0;

    lexer->pos++;
    while (isdigit((unsigned char)lexer->input[lexer->pos])) {
        value = (value * 10) + (lexer->input[lexer->pos] - '0');
        lexer->pos++;
    }

    Token token = make_token(TOKEN_NUMBER, lexer->input + start,
        lexer->pos - start);
    token.value = -value;
    return token;
}

static Token lex_identifier(Lexer *lexer)
{
    size_t start = lexer->pos;

    lexer->pos++;
    while (is_ident_continue((unsigned char)lexer->input[lexer->pos])) {
        lexer->pos++;
    }

    return make_token(TOKEN_IDENT, lexer->input + start, lexer->pos - start);
}

static Token lex_punctuator(Lexer *lexer)
{
    size_t start = lexer->pos;
    char ch = lexer->input[lexer->pos];

    if (ch == '=' && lexer->input[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '!' && lexer->input[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '('
        || ch == '%' || ch == ')' || ch == '{' || ch == '}' || ch == ';'
        || ch == ',' || ch == '=') {
        lexer->pos++;
        return make_token(TOKEN_PUNCT, lexer->input + start, 1);
    }

    lexer->pos++;
    return make_token(TOKEN_INVALID, lexer->input + start, 1);
}

Token lexer_next(Lexer *lexer)
{
    skip_whitespace(lexer);

    if (lexer->input[lexer->pos] == '\0') {
        return make_token(TOKEN_EOF, lexer->input + lexer->pos, 0);
    }

    if (isdigit((unsigned char)lexer->input[lexer->pos])) {
        return lex_number(lexer);
    }

    if (lexer->input[lexer->pos] == '-'
        && lexer->input[lexer->pos + 1] != '\0'
        && isdigit((unsigned char)lexer->input[lexer->pos + 1])) {
        return lex_negative_number(lexer);
    }

    if (is_ident_start((unsigned char)lexer->input[lexer->pos])) {
        return lex_identifier(lexer);
    }

    return lex_punctuator(lexer);
}
