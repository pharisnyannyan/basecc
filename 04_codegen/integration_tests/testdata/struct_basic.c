struct Pair {
  int left;
  int right;
};

struct Pair g_pair;

int struct_smoke() {
  g_pair.left = 10;
  g_pair.right = 20;
  return g_pair.left + g_pair.right;
}

int struct_set_and_get(int a, int b) {
  struct Pair p;
  p.left = a;
  p.right = b;
  return p.left * p.right;
}
