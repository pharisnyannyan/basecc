#include "checker.h"

#include <stdio.h>

int main(void) {
  const char *source = "int value = 7;";
  Checker checker;

  checker_init(&checker, source);

  if (!checker_check(&checker)) {
    fprintf(stderr, "checker error: %s\n", checker_error(&checker));
    return 1;
  }

  printf("syntax ok\n");
  return 0;
}
