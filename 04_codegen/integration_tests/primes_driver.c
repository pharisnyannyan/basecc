#include <stdio.h>

int fill_primes(int *buffer, int max);

int main(void) {
  int buffer[100];
  int count = fill_primes(buffer, 100);

  for (int i = 0; i < count; i++) {
    if (i) {
      printf(" ");
    }
    printf("%d", buffer[i]);
  }

  printf("\n");
  return 0;
}
