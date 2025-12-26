int sieve_primes(int *buffer, int max) {
  int flags[128];
  int i = 0;
  int count = 0;

  for (i = 0; max - i + 1; i = i + 1) {
    flags[i] = 1;
  }

  flags[0] = 0;
  flags[1] = 0;

  for (int p = 2; p - 8; p = p + 1) {
    if (flags[p]) {
      for (int m = p + p; max - m + 1; m = m + 1) {
        if (!(m % p)) {
          flags[m] = 0;
        }
      }
    }
  }

  for (i = 2; max - i + 1; i = i + 1) {
    if (flags[i]) {
      buffer[count] = i;
      count = count + 1;
    }
  }

  return count;
}
