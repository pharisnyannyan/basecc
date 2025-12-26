#include <stdio.h>

int extern_calls(void);

int main(void)
{
    printf("extern_calls=%d\n", extern_calls());
    return 0;
}
