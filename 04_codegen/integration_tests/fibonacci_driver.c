#include <stdio.h>

int fib_recursive(void);
int fib_iterative(void);

int main(void)
{
    printf("fib_recursive=%d\n", fib_recursive());
    printf("fib_iterative=%d\n", fib_iterative());
    return 0;
}
