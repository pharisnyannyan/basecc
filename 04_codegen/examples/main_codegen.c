#include "codegen.h"

#include <stdio.h>

int main(void) {
  const char *source = "int value = 7;";
  const char *output_path = "build/example.ll";
  Codegen codegen;

  codegen_init(&codegen, source);

  if (!codegen_emit(&codegen, output_path)) {
    fprintf(stderr, "codegen error: %s\n", codegen_error(&codegen));
    return 1;
  }

  printf("wrote %s\n", output_path);
  return 0;
}
