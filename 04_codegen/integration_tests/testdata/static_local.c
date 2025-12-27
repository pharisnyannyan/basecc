int counter() {
  static int count = 0;
  count = count + 1;
  return count;
}

int counter_init() {
  static int count = 42;
  count = count + 1;
  return count;
}
