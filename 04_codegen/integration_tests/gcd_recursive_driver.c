#include <stdio.h>

int gcd_recursive(int a, int b);

int main(void) {
  printf("%d %d\n", gcd_recursive(48, 18), gcd_recursive(270, 192));
  return 0;
}
