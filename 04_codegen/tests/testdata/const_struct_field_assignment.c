struct Pair {
  const int left;
  int right;
};

int main() {
  struct Pair pair;

  pair.left = 3;
  pair.right = 4;

  return pair.left + pair.right;
}
