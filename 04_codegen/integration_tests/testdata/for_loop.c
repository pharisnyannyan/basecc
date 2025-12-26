int sum_down() {
  int sum = 0;

  for (int i = 5; i; i = i - 1) {
    sum = sum + i;
  }

  return sum;
}
