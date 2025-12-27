struct S1 {
  int a;
  int b;
};

int size_int() {
  return sizeof(int);
}

int size_s1() {
  return sizeof(struct S1);
}

int size_s1_ptr() {
  return sizeof(struct S1 *);
}

int size_val() {
  int x;
  return sizeof(x);
}
