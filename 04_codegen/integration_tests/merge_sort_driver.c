#include <stdio.h>
#include <stdlib.h>

int merge_sort(int *values, int *temp, int low, int high);

static void fill_random(int *values, int count) {
  for (int i = 0; i < count; i++) {
    values[i] = rand() % 2;
  }
}

static int is_sorted(const int *values, int count) {
  for (int i = 1; i < count; i++) {
    if (values[i - 1] > values[i]) {
      return 0;
    }
  }

  return 1;
}

int main(void) {
  int values[10007];
  int temp[10007];
  int count = 10007;

  srand(1337);
  fill_random(values, count);

  merge_sort(values, temp, 0, count - 1);

  if (!is_sorted(values, count)) {
    printf("fail\n");
    return 1;
  }

  printf("ok\n");
  return 0;
}
