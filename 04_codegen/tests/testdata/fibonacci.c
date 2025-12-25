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
    int index = 0;
    int current = 0;
    int next = 1;

    while (index - n) {
        int temp = current + next;
        current = next;
        next = temp;
        index = index + 1;
    }

    return next;
}
