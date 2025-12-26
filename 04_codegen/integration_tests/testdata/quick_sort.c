int is_leq(int value, int pivot) {
  if (pivot) {
    return 1;
  }

  if (value) {
    return 0;
  }

  return 1;
}

int swap_values(int *left, int *right) {
  int temp = *left;
  *left = *right;
  *right = temp;
  return 0;
}

int partition(int *values, int low, int high) {
  int pivot = *(values + high);
  int i = low - 1;
  int j = low;
  int remaining = high - low;
  int ignored = 0;

  while (remaining) {
    if (is_leq(*(values + j), pivot)) {
      i = i + 1;
      ignored = swap_values(values + i, values + j);
    }
    j = j + 1;
    remaining = remaining - 1;
  }

  ignored = swap_values(values + i + 1, values + high);
  return i + 1;
}

int quick_sort(int *values, int low, int high) {
  int pivot_index = 0;
  int left_size = 0;
  int right_size = 0;
  int ignored = 0;

  if (!(high - low)) {
    return 0;
  }

  pivot_index = partition(values, low, high);

  left_size = pivot_index - low;
  if (left_size) {
    ignored = quick_sort(values, low, pivot_index - 1);
  }

  right_size = high - pivot_index;
  if (right_size) {
    ignored = quick_sort(values, pivot_index + 1, high);
  }

  return 0;
}
