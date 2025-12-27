#include <stdio.h>

enum Color { RED, GREEN, BLUE = 10, YELLOW };

int get_red();
int get_blue();
int get_yellow();
int test_enum_logic();

int main() {
  printf("red=%d\n", get_red());
  printf("blue=%d\n", get_blue());
  printf("yellow=%d\n", get_yellow());
  printf("enum_logic=%d\n", test_enum_logic());
  return 0;
}
