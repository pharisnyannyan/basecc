int fib_recursive(int n) {
    if (n) {
        if (n - 1) {
            return fib_recursive(n - 1) + fib_recursive(n - 2);
        }
    }

    return n;
}

int fib_iterative(int n) {
    int a = 0;
    int b = 1;
    int i = n;
    int next = 0;

    while (i) {
        next = a + b;
        a = b;
        b = next;
        i = i - 1;
    }

    return a;
}
