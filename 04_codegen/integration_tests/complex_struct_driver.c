#include <stddef.h>
#include <stdio.h>

struct Node {
  int value;
  struct Node *next;
};

int sum_data();
int get_second_value(struct Node *head);

int main() {
  printf("sum_data=%d\n", sum_data());

  struct Node n1, n2, n3;
  n1.value = 10;
  n1.next = &n2;
  n2.value = 20;
  n2.next = &n3;
  n3.value = 30;
  n3.next = NULL;

  printf("second_value=%d\n", get_second_value(&n1));
  return 0;
}
