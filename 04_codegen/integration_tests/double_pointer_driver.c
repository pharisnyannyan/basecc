#include <stdio.h>

int set_value(int **target, int value);
int increment_via_pointer(int **target);

int main(void) {
  int value = 5;
  int *ptr = &value;
  int **double_ptr = &ptr;

  int set_result = set_value(double_ptr, 42);
  int inc_result = increment_via_pointer(double_ptr);

  printf("value=%d\n", value);
  printf("set_result=%d\n", set_result);
  printf("inc_result=%d\n", inc_result);
  return 0;
}
