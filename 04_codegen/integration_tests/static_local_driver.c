#include <stdio.h>

int counter();
int counter_init();

int main() {
  printf("c1=%d\n", counter());
  printf("c2=%d\n", counter());
  printf("c3=%d\n", counter());
  printf("ci1=%d\n", counter_init());
  printf("ci2=%d\n", counter_init());
  return 0;
}
