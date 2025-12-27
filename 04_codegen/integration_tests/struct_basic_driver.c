#include <stdio.h>

int struct_smoke(void);
int struct_set_and_get(int a, int b);

int main(void) {
  printf("struct_smoke=%d\n", struct_smoke());
  printf("struct_set_and_get=%d\n", struct_set_and_get(5, 6));
  return 0;
}
