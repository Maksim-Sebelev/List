#include <stdio.h>


int main()
{
    size_t a = 1 << (sizeof(size_t) * 8 - 1);
    printf("a = %lu\n", a);
    return 0;
}