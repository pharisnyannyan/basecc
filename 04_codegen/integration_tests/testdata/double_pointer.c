int set_value(int **target, int value) {
  **target = value;
  return **target;
}

int increment_via_pointer(int **target) {
  **target = **target + 1;
  return **target;
}
