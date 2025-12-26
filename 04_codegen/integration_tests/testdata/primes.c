int fill_primes(int *buffer, int max) {
  int count = 0;
  int n = 2;

  while (1) {
    if (!(max - n + 1)) {
      break;
    }

    int is_prime = 1;

    for (int i = 2; n - i; i = i + 1) {
      if (!(n % i)) {
        is_prime = 0;
      }
    }

    if (is_prime) {
      *(buffer + count) = n;
      count = count + 1;
    }

    n = n + 1;
  }

  return count;
}
