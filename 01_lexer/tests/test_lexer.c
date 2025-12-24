#include "lexer.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *current_test = "unknown";

static void begin_test(const char *name)
{
    current_test = name;
}

static void failf(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "test failure (%s): ", current_test);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
    exit(1);
}

static void assert_true(int condition, const char *message)
{
    if (!condition) {
        failf("%s", message);
    }
}

static void assert_token_text(Token token, const char *text)
{
    size_t length = strlen(text);

    assert_true(token.length == length, "unexpected token length");
    assert_true(strncmp(token.start, text, length) == 0,
        "unexpected token text");
}

static void assert_punct_token(Token token, const char *text)
{
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_token_text(token, text);
}
static void test_ident_and_number(void)
{
    Lexer lexer;
    Token token;

    begin_test("identifiers and numbers");
    lexer_init(&lexer, "foo bar1 42");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_true(token.length == 3, "expected length 3");
    assert_true(strncmp(token.start, "foo", 3) == 0, "expected text 'foo'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_true(token.length == 4, "expected length 4");
    assert_true(strncmp(token.start, "bar1", 4) == 0, "expected text 'bar1'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "42", 2) == 0, "expected text '42'");
    if (token.value != 42) {
        failf("expected number value 42 but got %ld", token.value);
    }

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_punctuators(void)
{
    Lexer lexer;
    Token token;

    begin_test("punctuators");
    lexer_init(&lexer, "()+-*/%;,{ }==!= =");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "(", 1) == 0, "expected text '('");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, ")", 1) == 0, "expected text ')'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "+", 1) == 0, "expected text '+'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "-", 1) == 0, "expected text '-'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "*", 1) == 0, "expected text '*'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "/", 1) == 0, "expected text '/'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "%", 1) == 0, "expected text '%'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, ";", 1) == 0, "expected text ';'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, ",", 1) == 0, "expected text ','");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "{", 1) == 0, "expected text '{'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "}", 1) == 0, "expected text '}'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "==", 2) == 0, "expected text '=='");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "!=", 2) == 0, "expected text '!='");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "=", 1) == 0, "expected text '='");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_identifiers_with_underscores(void)
{
    Lexer lexer;
    Token token;

    begin_test("identifiers with underscores");
    lexer_init(&lexer, "_x __y1");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "_x", 2) == 0, "expected text '_x'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_true(token.length == 4, "expected length 4");
    assert_true(strncmp(token.start, "__y1", 4) == 0, "expected text '__y1'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_number_boundaries(void)
{
    Lexer lexer;
    Token token;

    begin_test("number boundaries");
    lexer_init(&lexer, "0 00123");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "0", 1) == 0, "expected text '0'");
    assert_true(token.value == 0, "expected value 0");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 5, "expected length 5");
    assert_true(strncmp(token.start, "00123", 5) == 0, "expected text '00123'");
    assert_true(token.value == 123, "expected value 123");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_negative_numbers(void)
{
    Lexer lexer;
    Token token;

    begin_test("negative numbers");
    lexer_init(&lexer, "-7 -0 - 8");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "-7", 2) == 0, "expected text '-7'");
    assert_true(token.value == -7, "expected value -7");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 2, "expected length 2");
    assert_true(strncmp(token.start, "-0", 2) == 0, "expected text '-0'");
    assert_true(token.value == 0, "expected value 0");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "-", 1) == 0, "expected text '-'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "8", 1) == 0, "expected text '8'");
    assert_true(token.value == 8, "expected value 8");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_invalid_character(void)
{
    Lexer lexer;
    Token token;

    begin_test("invalid character");
    lexer_init(&lexer, "@");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_INVALID, "expected TOKEN_INVALID");
    assert_true(token.length == 1, "expected length 1");
    assert_true(strncmp(token.start, "@", 1) == 0, "expected text '@'");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_sample_program(void)
{
    Lexer lexer;
    Token token;

    begin_test("sample program");
    lexer_init(&lexer, "int main(){int x=-7%3;return x;}");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "int");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "main");

    token = lexer_next(&lexer);
    assert_punct_token(token, "(");

    token = lexer_next(&lexer);
    assert_punct_token(token, ")");

    token = lexer_next(&lexer);
    assert_punct_token(token, "{");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "int");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "x");

    token = lexer_next(&lexer);
    assert_punct_token(token, "=");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_token_text(token, "-7");
    assert_true(token.value == -7, "expected value -7");

    token = lexer_next(&lexer);
    assert_punct_token(token, "%");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    assert_token_text(token, "3");
    assert_true(token.value == 3, "expected value 3");

    token = lexer_next(&lexer);
    assert_punct_token(token, ";");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "return");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    assert_token_text(token, "x");

    token = lexer_next(&lexer);
    assert_punct_token(token, ";");

    token = lexer_next(&lexer);
    assert_punct_token(token, "}");

    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

static void test_whitespace_only(void)
{
    Lexer lexer;
    Token token;

    begin_test("whitespace");
    lexer_init(&lexer, " \n\t  ");
    token = lexer_next(&lexer);
    assert_true(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    assert_true(token.length == 0, "expected empty EOF token");
}

int main(void)
{
    test_ident_and_number();
    test_punctuators();
    test_identifiers_with_underscores();
    test_number_boundaries();
    test_negative_numbers();
    test_invalid_character();
    test_sample_program();
    test_whitespace_only();
    printf("ok\n");
    return 0;
}
