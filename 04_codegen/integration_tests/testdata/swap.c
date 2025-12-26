int swap(int *left, int *right) {
  int temp = *left;
  *left = *right;
  *right = temp;
  return 0;
}
