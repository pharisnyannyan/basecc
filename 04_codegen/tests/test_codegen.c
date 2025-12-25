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

typedef struct {
    const char *name;
    const char *input_path;
    const char *expected_path;
} CodegenTestdata;

static int run_codegen_testdata(const CodegenTestdata *testdata)
{
    Codegen codegen;
    char output[256];
    char *source = NULL;
    char *expected = NULL;
    char *content = NULL;
    int passed = 0;

    source = read_file(testdata->input_path);
    if (!source) {
        failf("expected testdata input");
        goto cleanup;
    }

    expected = read_file(testdata->expected_path);
    if (!expected) {
        failf("expected testdata output");
        goto cleanup;
    }

    snprintf(output, sizeof(output), "build/%s.ll", testdata->name);

    codegen_init(&codegen, source);

    if (!codegen_emit(&codegen, output)) {
        failf("expected codegen success");
        goto cleanup;
    }
    if (codegen_error(&codegen) != NULL) {
        failf("unexpected codegen error");
        goto cleanup;
    }

    content = read_file(output);
    if (!content) {
        failf("expected output file content");
        goto cleanup;
    }
    if (strcmp(content, expected) != 0) {
        failf("unexpected LLVM IR output");
        goto cleanup;
    }

    passed = 1;

cleanup:
    free(source);
    free(expected);
    free(content);
    return passed;
}

TEST(generate_simple_module, "generate simple module")
{
    CodegenTestdata testdata = {
        "codegen_simple",
    return run_codegen_testdata(&testdata);
    CodegenTestdata testdata = {
    };

    return run_codegen_testdata(&testdata);
}

    CodegenTestdata testdata = {
    return run_codegen_testdata(&testdata);
    CodegenTestdata testdata = {
    return run_codegen_testdata(&testdata);
        failf("expected testdata input");
        "tests/fixtures/defaults.ll"
    };

    return run_codegen_fixture(&fixture);
}

TEST(generate_control_flow_function, "generate control flow function")
{
    CodegenFixture fixture = {
        "codegen_control_flow",
        "tests/fixtures/control_flow.c",
        "tests/fixtures/control_flow.ll"
    };

    return run_codegen_fixture(&fixture);
}

TEST(generate_function_call, "generate function call")
{
    CodegenFixture fixture = {
        "codegen_function_call",
        "tests/fixtures/function_call.c",
        "tests/fixtures/function_call.ll"
    };

    return run_codegen_fixture(&fixture);
}

TEST(check_invalid_syntax, "reject invalid syntax")
{
    Codegen codegen;
    char *source = NULL;

    source = read_file("tests/fixtures/invalid_syntax.c");
    if (!source) {
        failf("expected fixture input");
        return 0;
    }

    codegen_init(&codegen, source);

    ASSERT_TRUE(!codegen_emit(&codegen, "build/test_codegen_invalid.ll"),
        "expected codegen failure");
    ASSERT_TRUE(error_contains(codegen_error(&codegen), "expected expression"),
        "expected expression error");

    free(source);
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
