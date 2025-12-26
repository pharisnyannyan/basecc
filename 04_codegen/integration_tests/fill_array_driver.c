#include <stdio.h>

int fill_array(int *buffer, int n);

int main(void) {
  int buffer[6];
  int n = 6;

  fill_array(buffer, n);

  for (int i = 0; i < n; i++) {
    if (i) {
      printf(" ");
    }
    printf("%d", buffer[i]);
  }

  printf("\n");
  return 0;
}
