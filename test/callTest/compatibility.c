#include <stdio.h>

long int test(long int a,long int b, long int c, long int d, long int e, long int f, long int g, long int h, long int i)
{
    printf("%ld\n", a);
    printf("%ld\n", b);
    printf("%ld\n", c);

    printf("%ld\n", d);
    printf("%ld\n", e);
    printf("%ld\n", f);

    printf("%ld\n", g);
    printf("%ld\n", h);
    printf("%ld\n", i);

    return 2;
}

void testReg(long int a,long int b, long int c, long int d, long int e, long int f)
{
    printf("%ld\n", a);
    printf("%ld\n", b);
    printf("%ld\n", c);
    printf("%ld\n", d);
    printf("%ld\n", e);
    printf("%ld\n", f);
}

void printLongInt(long int a)
{
       printf("%ld\n", a);
}