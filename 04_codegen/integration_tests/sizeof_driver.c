#include <stdio.h>

int size_int();
int size_s1();
int size_s1_ptr();
int size_val();

int main() {
  printf("size_int=%d\n", size_int());
  printf("size_s1=%d\n", size_s1());
  printf("size_s1_ptr=%d\n", size_s1_ptr());
  printf("size_val=%d\n", size_val());
  return 0;
}
