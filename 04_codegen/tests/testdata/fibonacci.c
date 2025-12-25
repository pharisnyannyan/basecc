int fib(int n) {
    if (n && (n - 1)) {
        return fib(n - 1) + fib(n - 2);
    }

    return n;
}

int fib_recursive(int n) {
    return fib(n);
}

int fib_iterative(int n) {
    while (n) {
        return fib(n);
    }

    return 0;
}
