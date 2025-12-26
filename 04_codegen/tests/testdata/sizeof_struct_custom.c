struct Custom {
  char a;
  int b;
  short c;
};

int size_custom_type() {
  return sizeof(struct Custom);
}

int size_custom_value() {
  struct Custom value;
  return sizeof(value);
}
