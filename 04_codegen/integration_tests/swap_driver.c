#include <stdio.h>

int swap(int *left, int *right);

int main(void)
{
    int a = 3;
    int b = 7;

    swap(&a, &b);

    printf("a=%d\n", a);
    printf("b=%d\n", b);
    return 0;
}
