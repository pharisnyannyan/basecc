#include "lexer.h"

#include "test_util.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *current_test = "unknown";
static int total_passed = 0;
static int total_tests = 0;

static void begin_test(const char *name)
{
    current_test = name;
    total_tests++;
}

static void failf(const char *fmt, ...)
{
    va_list args;

    fprintf(stderr, "test failure (%s): ", current_test);
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X) \
    X(ident_and_number, "identifiers and numbers") \
    X(punctuators, "punctuators") \
    X(identifiers_with_underscores, "identifiers with underscores") \
    X(number_boundaries, "number boundaries") \
    X(negative_numbers, "negative numbers") \
    X(keywords, "keywords") \
    X(keyword_snippets, "keyword snippets") \
    X(invalid_character, "invalid character") \
    X(sample_program, "sample program") \
    X(whitespace_only, "whitespace")

#define ASSERT_PUNCT_TOKEN(token_value, text_value) \
    do { \
        ASSERT_TRUE((token_value).type == TOKEN_PUNCT, "expected TOKEN_PUNCT"); \
        ASSERT_TOKEN_TEXT((token_value), (text_value)); \
    } while (0)

#define ASSERT_KEYWORD_TOKEN(token_value, expected_type, text_value) \
    do { \
        ASSERT_TRUE((token_value).type == (expected_type), \
            "unexpected keyword token type"); \
        ASSERT_TOKEN_TEXT((token_value), (text_value)); \
    } while (0)
TEST(ident_and_number, "identifiers and numbers")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "foo bar1 42");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TRUE(token.length == 3, "expected length 3");
    ASSERT_TRUE(strncmp(token.start, "foo", 3) == 0, "expected text 'foo'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TRUE(token.length == 4, "expected length 4");
    ASSERT_TRUE(strncmp(token.start, "bar1", 4) == 0, "expected text 'bar1'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "42", 2) == 0, "expected text '42'");
    ASSERT_TOKEN_VALUE(token, 42);

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(punctuators, "punctuators")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "()+-*/%;,{ }==!= =");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "(", 1) == 0, "expected text '('");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, ")", 1) == 0, "expected text ')'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "+", 1) == 0, "expected text '+'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "-", 1) == 0, "expected text '-'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "*", 1) == 0, "expected text '*'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "/", 1) == 0, "expected text '/'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "%", 1) == 0, "expected text '%'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, ";", 1) == 0, "expected text ';'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, ",", 1) == 0, "expected text ','");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "{", 1) == 0, "expected text '{'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "}", 1) == 0, "expected text '}'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "==", 2) == 0, "expected text '=='");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "!=", 2) == 0, "expected text '!='");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "=", 1) == 0, "expected text '='");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(identifiers_with_underscores, "identifiers with underscores")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "_x __y1");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "_x", 2) == 0, "expected text '_x'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TRUE(token.length == 4, "expected length 4");
    ASSERT_TRUE(strncmp(token.start, "__y1", 4) == 0, "expected text '__y1'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(number_boundaries, "number boundaries")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "0 00123");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "0", 1) == 0, "expected text '0'");
    ASSERT_TRUE(token.value == 0, "expected value 0");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 5, "expected length 5");
    ASSERT_TRUE(strncmp(token.start, "00123", 5) == 0, "expected text '00123'");
    ASSERT_TRUE(token.value == 123, "expected value 123");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(negative_numbers, "negative numbers")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "-7 -0 - 8");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "-7", 2) == 0, "expected text '-7'");
    ASSERT_TRUE(token.value == -7, "expected value -7");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 2, "expected length 2");
    ASSERT_TRUE(strncmp(token.start, "-0", 2) == 0, "expected text '-0'");
    ASSERT_TRUE(token.value == 0, "expected value 0");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_PUNCT, "expected TOKEN_PUNCT");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "-", 1) == 0, "expected text '-'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "8", 1) == 0, "expected text '8'");
    ASSERT_TRUE(token.value == 8, "expected value 8");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(keywords, "keywords")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "if else while switch case return iffy");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_IF, "if");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_ELSE, "else");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_WHILE, "while");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_SWITCH, "switch");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_CASE, "case");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "iffy");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(keyword_snippets, "keyword snippets")
{
    Lexer lexer;
    Token token;

    lexer_init(&lexer,
        "int classify(int x){\n"
        "if(x==0){return 0;}else{return 1;}\n"
        "}\n"
        "int loop(int n){while(n!=0){n=n-1;}return n;}\n"
        "int sw(int v){switch(v){case 0:return 0;case 1:return 1;}}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "classify");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "x");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_IF, "if");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "x");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "==");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "0");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "0");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_ELSE, "else");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "1");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "loop");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "n");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_WHILE, "while");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "n");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "!=");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "0");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "n");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "=");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "n");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "-1");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "n");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "sw");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "v");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_SWITCH, "switch");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "v");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_CASE, "case");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "0");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_INVALID, "expected TOKEN_INVALID");
    ASSERT_TOKEN_TEXT(token, ":");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "0");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_CASE, "case");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "1");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_INVALID, "expected TOKEN_INVALID");
    ASSERT_TOKEN_TEXT(token, ":");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "1");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(invalid_character, "invalid character")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "@");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_INVALID, "expected TOKEN_INVALID");
    ASSERT_TRUE(token.length == 1, "expected length 1");
    ASSERT_TRUE(strncmp(token.start, "@", 1) == 0, "expected text '@'");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(sample_program, "sample program")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, "int main(){int x=-7%3;return x;}");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "main");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "(");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ")");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "{");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_INT, "int");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "x");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "=");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "-7");
    ASSERT_TRUE(token.value == -7, "expected value -7");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "%");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_NUMBER, "expected TOKEN_NUMBER");
    ASSERT_TOKEN_TEXT(token, "3");
    ASSERT_TRUE(token.value == 3, "expected value 3");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_KEYWORD_TOKEN(token, TOKEN_RETURN, "return");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_IDENT, "expected TOKEN_IDENT");
    ASSERT_TOKEN_TEXT(token, "x");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, ";");

    token = lexer_next(&lexer);
    ASSERT_PUNCT_TOKEN(token, "}");

    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

TEST(whitespace_only, "whitespace")
{
    Lexer lexer;
    Token token;
    lexer_init(&lexer, " \n\t  ");
    token = lexer_next(&lexer);
    ASSERT_TRUE(token.type == TOKEN_EOF, "expected TOKEN_EOF");
    ASSERT_TRUE(token.length == 0, "expected empty EOF token");

    return 1;
}

typedef struct {
    const char *name;
    int (*fn)(void);
} TestCase;

#define TEST_ENTRY(name, description) { description, test_##name },

static const TestCase tests[] = {
    TEST_LIST(TEST_ENTRY)
};

int main(void)
{
    size_t i;

    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        begin_test(tests[i].name);
        if (tests[i].fn()) {
            total_passed++;
            printf("PASS: %s\n", current_test);
        } else {
            printf("FAIL: %s\n", current_test);
        }
    }
    printf("\nSummary: %d passed, %d failed, %d total\n",
        total_passed,
        total_tests - total_passed,
        total_tests);
    return total_tests == total_passed ? 0 : 1;
}
