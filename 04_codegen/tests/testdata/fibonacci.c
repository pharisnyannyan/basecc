int fib_recursive_n;

int fib_recursive_inner() {
    if (fib_recursive_n <= 1) {
        return fib_recursive_n;
    }

    fib_recursive_n = fib_recursive_n - 1;
    int left = fib_recursive_inner();

    fib_recursive_n = fib_recursive_n - 1;
    int right = fib_recursive_inner();

    fib_recursive_n = fib_recursive_n + 2;
    return left + right;
}

int fib_recursive() {
    fib_recursive_n = 10;
    return fib_recursive_inner();
}

int fib_iterative() {
    int target = 10;
    int a = 0;
    int b = 1;
    int i = 0;

    while (i < target) {
        int next = a + b;
        a = b;
        b = next;
        i = i + 1;
    }

    return a;
}
