#include "codegen.h"
#include "test_util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEST(name, description) static int test_##name(void)

#define TEST_LIST(X)                                                           \
  X(generate_simple_module, "generate simple module")                          \
  X(generate_type_declarations, "generate type declarations")                  \
  X(generate_defaults, "generate default initializers")                        \
  X(generate_static_storage, "generate static storage")                        \
  X(generate_pointer_globals, "generate pointer globals")                      \
  X(generate_array_ops, "generate array operations")                           \
  X(generate_pointer_return, "generate pointer return")                        \
  X(generate_typedef_casts, "generate typedef casts")                          \
  X(generate_struct_definitions, "generate struct definitions")                \
  X(generate_control_flow_function, "generate control flow function")          \
  X(generate_loop_control, "generate loop control")                            \
  X(generate_function_call, "generate function call")                          \
  X(generate_extern_calls, "generate extern calls")                            \
  X(generate_arithmetic_function, "generate arithmetic function")              \
  X(generate_logical_function, "generate logical operators")                   \
  X(generate_sizeof, "generate sizeof expressions")                            \
  X(generate_sizeof_struct_custom, "generate sizeof for custom struct")        \
  X(check_invalid_syntax, "reject invalid syntax")                             \
  X(check_const_assignment, "reject const assignment")                         \
  X(check_const_field_assignment, "reject const field assignment")

static char *read_file(const char *path, size_t *size_out) {
  FILE *file = fopen(path, "rb");
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

  if (size_out) {
    *size_out = read_bytes;
  }

  return buffer;
}

static void normalize_line_endings(char *buffer, size_t *size) {
  size_t read_index = 0;
  size_t write_index = 0;

  while (read_index < *size) {
    if (buffer[read_index] == '\r' && read_index + 1 < *size &&
        buffer[read_index + 1] == '\n') {
      read_index++;
    }

    buffer[write_index++] = buffer[read_index++];
  }

  *size = write_index;
  buffer[write_index] = '\0';
}

typedef struct {
  const char *name;
  const char *input_path;
  const char *expected_path;
} CodegenFixture;

static char *build_output_path(const char *name) {
  size_t length = (size_t)snprintf(NULL, 0, "build/%s.ll", name);
  char *path = malloc(length + 1);

  if (!path) {
    return NULL;
  }

  snprintf(path, length + 1, "build/%s.ll", name);
  return path;
}

static int run_codegen_fixture(const CodegenFixture *fixture) {
  Codegen codegen;
  char *source = NULL;
  char *expected = NULL;
  char *content = NULL;
  char *output_path = NULL;
  size_t expected_size = 0;
  size_t content_size = 0;
  int passed = 0;

  source = read_file(fixture->input_path, NULL);
  if (!source) {
    failf("expected fixture input");
    goto cleanup;
  }

  expected = read_file(fixture->expected_path, &expected_size);
  if (!expected) {
    failf("expected fixture output");
    goto cleanup;
  }

  output_path = build_output_path(fixture->name);
  if (!output_path) {
    failf("expected output path");
    goto cleanup;
  }

  codegen_init(&codegen, source);

  if (!codegen_emit(&codegen, output_path)) {
    failf("expected codegen success");
    goto cleanup;
  }
  if (codegen_error(&codegen) != NULL) {
    failf("unexpected codegen error");
    goto cleanup;
  }

  content = read_file(output_path, &content_size);
  if (!content) {
    failf("expected output file content");
    goto cleanup;
  }

  normalize_line_endings(expected, &expected_size);
  normalize_line_endings(content, &content_size);

  if (expected_size != content_size ||
      memcmp(content, expected, expected_size) != 0) {
    failf("unexpected LLVM IR output");
    goto cleanup;
  }

  passed = 1;

cleanup:
  free(source);
  free(expected);
  free(content);
  free(output_path);
  return passed;
}

TEST(generate_simple_module, "generate simple module") {
  CodegenFixture fixture = {"codegen_simple", "tests/testdata/simple_module.c",
                            "tests/testdata/simple_module.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_type_declarations, "generate type declarations") {
  CodegenFixture fixture = {"codegen_types", "tests/testdata/type_keywords.c",
                            "tests/testdata/type_keywords.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_defaults, "generate default initializers") {
  CodegenFixture fixture = {"codegen_defaults", "tests/testdata/defaults.c",
                            "tests/testdata/defaults.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_static_storage, "generate static storage") {
  CodegenFixture fixture = {"codegen_static_storage",
                            "tests/testdata/static_storage.c",
                            "tests/testdata/static_storage.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_pointer_globals, "generate pointer globals") {
  CodegenFixture fixture = {"codegen_pointers",
                            "tests/testdata/pointer_globals.c",
                            "tests/testdata/pointer_globals.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_array_ops, "generate array operations") {
  CodegenFixture fixture = {"codegen_array_ops", "tests/testdata/array_ops.c",
                            "tests/testdata/array_ops.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_pointer_return, "generate pointer return") {
  CodegenFixture fixture = {"codegen_pointer_return",
                            "tests/testdata/pointer_return.c",
                            "tests/testdata/pointer_return.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_typedef_casts, "generate typedef casts") {
  CodegenFixture fixture = {"codegen_typedef_casts",
                            "tests/testdata/typedef_casts.c",
                            "tests/testdata/typedef_casts.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_struct_definitions, "generate struct definitions") {
  CodegenFixture fixture = {"codegen_structs", "tests/testdata/struct_basic.c",
                            "tests/testdata/struct_basic.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_control_flow_function, "generate control flow function") {
  CodegenFixture fixture = {"codegen_control_flow",
                            "tests/testdata/control_flow.c",
                            "tests/testdata/control_flow.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_loop_control, "generate loop control") {
  CodegenFixture fixture = {"codegen_loop_control",
                            "tests/testdata/loop_control.c",
                            "tests/testdata/loop_control.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_function_call, "generate function call") {
  CodegenFixture fixture = {"codegen_function_call",
                            "tests/testdata/function_call.c",
                            "tests/testdata/function_call.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_extern_calls, "generate extern calls") {
  CodegenFixture fixture = {"codegen_extern_calls",
                            "tests/testdata/extern_calls.c",
                            "tests/testdata/extern_calls.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_arithmetic_function, "generate arithmetic function") {
  CodegenFixture fixture = {"codegen_arithmetic",
                            "tests/testdata/arithmetic_runtime.c",
                            "tests/testdata/arithmetic_runtime.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_logical_function, "generate logical operators") {
  CodegenFixture fixture = {"codegen_logical", "tests/testdata/logical_ops.c",
                            "tests/testdata/logical_ops.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_sizeof, "generate sizeof expressions") {
  CodegenFixture fixture = {"codegen_sizeof", "tests/testdata/sizeof_ops.c",
                            "tests/testdata/sizeof_ops.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(generate_sizeof_struct_custom, "generate sizeof for custom struct") {
  CodegenFixture fixture = {"codegen_sizeof_custom",
                            "tests/testdata/sizeof_struct_custom.c",
                            "tests/testdata/sizeof_struct_custom.ll"};

  return run_codegen_fixture(&fixture);
}

TEST(check_invalid_syntax, "reject invalid syntax") {
  Codegen codegen;
  char *source = read_file("tests/testdata/invalid_syntax.c", NULL);
  if (!source) {
    failf("expected fixture input 'tests/testdata/invalid_syntax.c'");
    return 0;
  }

  codegen_init(&codegen, source);

  ASSERT_TRUE(!codegen_emit(&codegen, "build/test_codegen_invalid.ll"),
              "expected codegen failure for invalid syntax");
  ASSERT_TRUE(
      test_error_contains(codegen_error(&codegen), "expected expression"),
      "expected 'expected expression' error message");

  free(source);
  return 1;
}

TEST(check_const_assignment, "reject const assignment") {
  Codegen codegen;
  char *source = read_file("tests/testdata/const_assignment.c", NULL);
  if (!source) {
    failf("expected fixture input 'tests/testdata/const_assignment.c'");
    return 0;
  }

  codegen_init(&codegen, source);

  ASSERT_TRUE(!codegen_emit(&codegen, "build/test_codegen_const.ll"),
              "expected codegen failure for const assignment");
  ASSERT_TRUE(test_error_contains(codegen_error(&codegen), "const"),
              "expected error containing 'const'");

  free(source);
  return 1;
}

TEST(check_const_field_assignment, "reject const field assignment") {
  Codegen codegen;
  char *source =
      read_file("tests/testdata/const_struct_field_assignment.c", NULL);
  if (!source) {
    failf("expected fixture input "
          "'tests/testdata/const_struct_field_assignment.c'");
    return 0;
  }

  codegen_init(&codegen, source);

  ASSERT_TRUE(!codegen_emit(&codegen, "build/test_codegen_const_field.ll"),
              "expected codegen failure for const field assignment");
  ASSERT_TRUE(test_error_contains(codegen_error(&codegen), "const"),
              "expected error containing 'const'");

  free(source);
  return 1;
}

#define TEST_ENTRY(name, description) {description, test_##name},

static const TestCase tests[] = {TEST_LIST(TEST_ENTRY)};

int main(void) { return test_run(tests, sizeof(tests) / sizeof(tests[0])); }
