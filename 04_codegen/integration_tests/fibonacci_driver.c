#include <stdio.h>

int fib_recursive(int n);
int fib_iterative(int n);

int main(void) {
  printf("fib_recursive=%d\n", fib_recursive(10));
  printf("fib_iterative=%d\n", fib_iterative(10));
  return 0;
}
