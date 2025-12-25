#include "codegen.h"
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
    X(generate_simple_module, "generate simple module") \
    X(generate_defaults, "generate default initializers") \
    X(generate_control_flow_function, "generate control flow function") \
    X(generate_function_call, "generate function call") \
    X(check_invalid_syntax, "reject invalid syntax")

static int error_contains(const char *error, const char *text)
{
    if (!error) {
        return 0;
    }

    return strstr(error, text) != NULL;
}

static char *read_file(const char *path)
{
    FILE *file = fopen(path, "r");
    char *buffer = NULL;
    long size = 0;
    size_t read_bytes = 0;

    if (!file) {
        return NULL;
    }

    if (fseek(file, 0, SEEK_END) != 0) {
        fclose(file);
        return NULL;
    }

    size = ftell(file);
    if (size < 0) {
        fclose(file);
        return NULL;
    }

    if (fseek(file, 0, SEEK_SET) != 0) {
        fclose(file);
        return NULL;
    }

    buffer = malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }

    read_bytes = fread(buffer, 1, (size_t)size, file);
    buffer[read_bytes] = '\0';

    fclose(file);
    return buffer;
}

TEST(generate_simple_module, "generate simple module")
{
    Codegen codegen;
    const char *output = "build/test_codegen_simple.ll";
    const char *expected =
        "; ModuleID = 'basecc'\n"
        "source_filename = \"basecc\"\n\n"
        "@value = global i32 7\n";
    char *content = NULL;

    codegen_init(&codegen, "int value = 7;");

    ASSERT_TRUE(codegen_emit(&codegen, output),
        "expected codegen success");
    ASSERT_TRUE(codegen_error(&codegen) == NULL,
        "unexpected codegen error");

    content = read_file(output);
    ASSERT_TRUE(content != NULL, "expected output file content");
    ASSERT_TRUE(strcmp(content, expected) == 0,
        "unexpected LLVM IR output");

    free(content);
    return 1;
}

TEST(generate_defaults, "generate default initializers")
{
    Codegen codegen;
    const char *output = "build/test_codegen_defaults.ll";
    const char *expected =
        "; ModuleID = 'basecc'\n"
        "source_filename = \"basecc\"\n\n"
        "@main = global i32 0\n"
        "@value = global i32 -3\n";
    char *content = NULL;

    codegen_init(&codegen, "int main; int value = -3;");

    ASSERT_TRUE(codegen_emit(&codegen, output),
        "expected codegen success");
    ASSERT_TRUE(codegen_error(&codegen) == NULL,
        "unexpected codegen error");

    content = read_file(output);
    ASSERT_TRUE(content != NULL, "expected output file content");
    ASSERT_TRUE(strcmp(content, expected) == 0,
        "unexpected LLVM IR output");

    free(content);
    return 1;
}

TEST(generate_control_flow_function, "generate control flow function")
{
    Codegen codegen;
    const char *output = "build/test_codegen_control_flow.ll";
    const char *expected =
        "; ModuleID = 'basecc'\n"
        "source_filename = \"basecc\"\n\n"
        "define i32 @main() {\n"
        "entry:\n"
        "  br label %while.cond0\n"
        "while.cond0:\n"
        "  %t0 = icmp ne i32 0, 0\n"
        "  br i1 %t0, label %while.body1, label %while.end2\n"
        "while.body1:\n"
        "  br label %while.cond0\n"
        "while.end2:\n"
        "  %t1 = icmp ne i32 1, 0\n"
        "  br i1 %t1, label %if.then3, label %if.else4\n"
        "if.then3:\n"
        "  ret i32 2\n"
        "if.else4:\n"
        "  ret i32 3\n"
        "}\n";
    char *content = NULL;

    codegen_init(&codegen,
        "int main(){while(0);if(1){return 2;}else{return 3;}}");

    ASSERT_TRUE(codegen_emit(&codegen, output),
        "expected codegen success");
    ASSERT_TRUE(codegen_error(&codegen) == NULL,
        "unexpected codegen error");

    content = read_file(output);
    ASSERT_TRUE(content != NULL, "expected output file content");
    ASSERT_TRUE(strcmp(content, expected) == 0,
        "unexpected LLVM IR output");

    free(content);
    return 1;
}

TEST(generate_function_call, "generate function call")
{
    Codegen codegen;
    const char *output = "build/test_codegen_call.ll";
    const char *expected =
        "; ModuleID = 'basecc'\n"
        "source_filename = \"basecc\"\n\n"
        "define i32 @foo() {\n"
        "entry:\n"
        "  ret i32 7\n"
        "}\n"
        "define i32 @main() {\n"
        "entry:\n"
        "  %t0 = call i32 @foo()\n"
        "  ret i32 %t0\n"
        "}\n";
    char *content = NULL;

    codegen_init(&codegen, "int foo(){return 7;} int main(){return foo();}");

    ASSERT_TRUE(codegen_emit(&codegen, output),
        "expected codegen success");
    ASSERT_TRUE(codegen_error(&codegen) == NULL,
        "unexpected codegen error");

    content = read_file(output);
    ASSERT_TRUE(content != NULL, "expected output file content");
    ASSERT_TRUE(strcmp(content, expected) == 0,
        "unexpected LLVM IR output");

    free(content);
    return 1;
}

TEST(check_invalid_syntax, "reject invalid syntax")
{
    Codegen codegen;

    codegen_init(&codegen, "int value = ;");

    ASSERT_TRUE(!codegen_emit(&codegen, "build/test_codegen_invalid.ll"),
        "expected codegen failure");
    ASSERT_TRUE(error_contains(codegen_error(&codegen), "expected expression"),
        "expected expression error");

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
