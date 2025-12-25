int fib_recursive_0() {
    return 0;
}

int fib_recursive_1() {
    return 1;
}

int fib_recursive_2() {
    return fib_recursive_1() + fib_recursive_0();
}

int fib_recursive_3() {
    return fib_recursive_2() + fib_recursive_1();
}

int fib_recursive_4() {
    return fib_recursive_3() + fib_recursive_2();
}

int fib_recursive_5() {
    return fib_recursive_4() + fib_recursive_3();
}

int fib_recursive_6() {
    return fib_recursive_5() + fib_recursive_4();
}

int fib_recursive_7() {
    return fib_recursive_6() + fib_recursive_5();
}

int fib_recursive_8() {
    return fib_recursive_7() + fib_recursive_6();
}

int fib_recursive_9() {
    return fib_recursive_8() + fib_recursive_7();
}

int fib_recursive_10() {
    return fib_recursive_9() + fib_recursive_8();
}

int fib_recursive() {
    return fib_recursive_10();
}

int fib_iterative() {
    while (1) {
        return fib_recursive_10();
    }

    return 0;
}
