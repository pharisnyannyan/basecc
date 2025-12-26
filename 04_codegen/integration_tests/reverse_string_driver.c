#include <stdio.h>
#include <string.h>

int reverse_string(char *buffer, int length);

int main(void) {
  char message[] = "basecc";

  reverse_string(message, (int)strlen(message));
  printf("reverse=%s\n", message);
  return 0;
}
