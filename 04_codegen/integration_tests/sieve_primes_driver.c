#include <stdio.h>

int sieve_primes(int *buffer, int max);

int main(void)
{
    int buffer[64];
    int count = sieve_primes(buffer, 50);

    for (int i = 0; i < count; i++) {
        if (i) {
            printf(" ");
        }
        printf("%d", buffer[i]);
    }

    printf("\n");
    return 0;
}
