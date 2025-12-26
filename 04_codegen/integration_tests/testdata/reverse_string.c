int reverse_string(char *buffer, int length) {
  int left = 0;
  int right = length - 1;
  int swaps = length / 2;

  while (swaps) {
    char temp = *(buffer + left);
    *(buffer + left) = *(buffer + right);
    *(buffer + right) = temp;
    left = left + 1;
    right = right - 1;
    swaps = swaps - 1;
  }

  return 0;
}
