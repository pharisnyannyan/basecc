int fib(int n) {
    if (n && (n - 1)) {
        return fib(n - 1) + fib(n - 2);
    }

    return n;
}

int fib_recursive() {
    return fib(6);
}

int fib_iterative() {
    while (1) {
        return fib(5);
    }

    return 0;
}
