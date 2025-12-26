#include <stdio.h>

int conv1d(int *a, int n, int *b, int m, int *out);

int main(void) {
  int a[3] = {1, 2, 3};
  int b[2] = {4, 5};
  int out[4];

  conv1d(a, 3, b, 2, out);

  for (int i = 0; i < 4; i++) {
    if (i) {
      printf(" ");
    }
    printf("%d", out[i]);
  }

  printf("\n");
  return 0;
}
