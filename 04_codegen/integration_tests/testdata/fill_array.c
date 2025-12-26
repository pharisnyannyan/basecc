int fill_array(int *buffer, int n) {
  for (int i = 0; n - i; i = i + 1) {
    *(buffer + i) = i;
  }

  return 0;
}
