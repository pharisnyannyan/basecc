#include <stdio.h>

int add(void);
int sub(void);
int mul(void);
int divide(void);
int mod(void);
int mixed(void);

int main(void)
{
    printf("add=%d\n", add());
    printf("sub=%d\n", sub());
    printf("mul=%d\n", mul());
    printf("divide=%d\n", divide());
    printf("mod=%d\n", mod());
    printf("mixed=%d\n", mixed());
    return 0;
}
