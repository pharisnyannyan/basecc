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

    size_t length = lexer->pos - start;
    const char *text = lexer->input + start;

    if (length == 2 && strncmp(text, "if", 2) == 0) {
        return make_token(TOKEN_IF, text, length);
    }

    if (length == 4 && strncmp(text, "else", 4) == 0) {
        return make_token(TOKEN_ELSE, text, length);
    }

    if (length == 5 && strncmp(text, "while", 5) == 0) {
        return make_token(TOKEN_WHILE, text, length);
    }

    if (length == 3 && strncmp(text, "for", 3) == 0) {
        return make_token(TOKEN_FOR, text, length);
    }

    if (length == 6 && strncmp(text, "switch", 6) == 0) {
        return make_token(TOKEN_SWITCH, text, length);
    }

    if (length == 4 && strncmp(text, "case", 4) == 0) {
        return make_token(TOKEN_CASE, text, length);
    }

    if (length == 5 && strncmp(text, "break", 5) == 0) {
        return make_token(TOKEN_BREAK, text, length);
    }

    if (length == 8 && strncmp(text, "continue", 8) == 0) {
        return make_token(TOKEN_CONTINUE, text, length);
    }

    if (length == 6 && strncmp(text, "return", 6) == 0) {
        return make_token(TOKEN_RETURN, text, length);
    }

    if (length == 6 && strncmp(text, "sizeof", 6) == 0) {
        return make_token(TOKEN_SIZEOF, text, length);
    }

    if (length == 4 && strncmp(text, "char", 4) == 0) {
        return make_token(TOKEN_CHAR, text, length);
    }

    if (length == 5 && strncmp(text, "short", 5) == 0) {
        return make_token(TOKEN_SHORT, text, length);
    }

    if (length == 3 && strncmp(text, "int", 3) == 0) {
        return make_token(TOKEN_INT, text, length);
    }

    if (length == 6 && strncmp(text, "struct", 6) == 0) {
        return make_token(TOKEN_STRUCT, text, length);
    }

    return make_token(TOKEN_IDENT, text, length);
}

static Token lex_punctuator(Lexer *lexer)
{
    size_t start = lexer->pos;
    char ch = lexer->input[lexer->pos];

    if (ch == '=' && lexer->input[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '-' && lexer->input[lexer->pos + 1] == '>') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '&' && lexer->input[lexer->pos + 1] == '&') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '&') {
        lexer->pos++;
        return make_token(TOKEN_PUNCT, lexer->input + start, 1);
    }

    if (ch == '|' && lexer->input[lexer->pos + 1] == '|') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '!' && lexer->input[lexer->pos + 1] == '=') {
        lexer->pos += 2;
        return make_token(TOKEN_PUNCT, lexer->input + start, 2);
    }

    if (ch == '!') {
        lexer->pos++;
        return make_token(TOKEN_PUNCT, lexer->input + start, 1);
    }

    if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '('
        || ch == '%' || ch == ')' || ch == '{' || ch == '}' || ch == ';'
        || ch == ',' || ch == '=' || ch == '.') {
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
