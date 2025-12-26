int is_leq(int left, int right) {
  if (right) {
    return 1;
  }

  if (left) {
    return 0;
  }

  return 1;
}

int merge_into(int *values, int *temp, int low, int mid, int high) {
  int i = low;
  int j = mid + 1;
  int k = low;
  int left_remaining = mid - i + 1;
  int right_remaining = high - j + 1;

  while (left_remaining && right_remaining) {
    if (is_leq(*(values + i), *(values + j))) {
      *(temp + k) = *(values + i);
      i = i + 1;
    } else {
      *(temp + k) = *(values + j);
      j = j + 1;
    }
    k = k + 1;
    left_remaining = mid - i + 1;
    right_remaining = high - j + 1;
  }

  left_remaining = mid - i + 1;
  while (left_remaining) {
    *(temp + k) = *(values + i);
    i = i + 1;
    k = k + 1;
    left_remaining = mid - i + 1;
  }

  right_remaining = high - j + 1;
  while (right_remaining) {
    *(temp + k) = *(values + j);
    j = j + 1;
    k = k + 1;
    right_remaining = high - j + 1;
  }

  int count = high - low + 1;
  for (int offset = 0; count - offset; offset = offset + 1) {
    *(values + low + offset) = *(temp + low + offset);
  }

  return 0;
}

int merge_sort(int *values, int *temp, int low, int high) {
  int mid = 0;
  int ignored = 0;

  if (!(high - low)) {
    return 0;
  }

  mid = low + (high - low) / 2;

  ignored = merge_sort(values, temp, low, mid);
  ignored = merge_sort(values, temp, mid + 1, high);
  ignored = merge_into(values, temp, low, mid, high);

  return 0;
}
