#include <stdint.h>

signed char test_signed_char(void)
{
    signed char r = 100;
    signed char b = 7;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 2;
    r = r >> 1;

    return r;
}

unsigned char test_unsigned_char(void)
{
    unsigned char r = 200;
    unsigned char b = 13;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 2;
    r = r >> 1;

    return r;
}

short test_short(void)
{
    short r = 1234;
    short b = 37;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 2;
    r = r >> 1;

    return r;
}

unsigned short test_unsigned_short(void)
{
    unsigned short r = 54321;
    unsigned short b = 123;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 2;
    r = r >> 1;

    return r;
}

int test_int(void)
{
    int r = 123456;
    int b = 789;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 3;
    r = r >> 2;

    return r;
}

unsigned int test_unsigned_int(void)
{
    unsigned int r = 4000000000u;
    unsigned int b = 12345u;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 3;
    r = r >> 2;

    return r;
}

long test_long(void)
{
    long r = 123456789L;
    long b = 1234L;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 4;
    r = r >> 3;

    return r;
}

unsigned long test_unsigned_long(void)
{
    unsigned long r = 3000000000UL;
    unsigned long b = 777UL;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 4;
    r = r >> 3;

    return r;
}

long long test_long_long(void)
{
    long long r = 1234567890123LL;
    long long b = 4567LL;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 5;
    r = r >> 4;

    return r;
}

unsigned long long test_unsigned_long_long(void)
{
    unsigned long long r = 123456789012345ULL;
    unsigned long long b = 98765ULL;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;
    r = r % b;
    r = r & b;
    r = r | b;
    r = r ^ b;
    r = r << 5;
    r = r >> 4;

    return r;
}

float test_float(void)
{
    float r = 3.5f;
    float b = 1.25f;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;

    return r;
}

double test_double(void)
{
    double r = 3.141592653589793;
    double b = 2.718281828459045;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;

    return r;
}

long double test_long_double(void)
{
    long double r = 1.234567890123456789L;
    long double b = 9.876543210987654321L;

    r = r + b;
    r = r - b;
    r = r * b;
    r = r / b;

    return r;
}

int test_int_mixed(void)
{
    int r = 1000;
    unsigned int u = 4000u;

    r = r + u;
    r = r - u;
    r = r * u;
    r = r / u;

    r = r + (int)u;
    r = r - (int)u;

    return r;
}

int test_unsigned_int_mixed(void)
{
    unsigned int r = 3000u;
    int s = -1200;

    r = r + s;
    r = r - s;
    r = r * s;
    r = r / (unsigned int)s;

    r = r + (unsigned int)s;

    return r;
}

int test_char_short_mixed(void)
{
    signed char sc = -10;
    unsigned char uc = 200;
    short s = 3000;
    unsigned short us = 60000;

    int r = sc;

    r = r + uc;
    r = r + s;
    r = r + us;

    r = r + (sc + uc);
    r = r + (s + us);

    return r;
}

long test_long_int_mixed(void)
{
    long r = 100000L;
    int i = -500;

    r = r + i;
    r = r - i;
    r = r * i;
    r = r / i;

    r = r + (long)i;

    return r;
}

unsigned long test_ulong_int_mixed(void)
{
    unsigned long r = 3000000000UL;
    int i = -1000;

    r = r + i;
    r = r - i;
    r = r * i;
    r = r / (unsigned long)i;

    r = r + (unsigned long)i;

    return r;
}

long long test_ll_mixed(void)
{
    long long r = 5000000000LL;
    unsigned long long u = 9000000000ULL;

    r = r + u;
    r = r - u;
    r = r * u;
    r = r / (long long)u;

    r = r + (long long)u;

    return r;
}

unsigned long long test_ull_mixed(void)
{
    unsigned long long r = 8000000000ULL;
    long long s = -3000LL;

    r = r + s;
    r = r - s;
    r = r * s;
    r = r / (unsigned long long)s;

    r = r + (unsigned long long)s;

    return r;
}

double test_double_int_mixed(void)
{
    double r = 3.14;
    int i = 10;
    unsigned int u = 20u;

    r = r + i;
    r = r + u;
    r = r - i;
    r = r * u;
    r = r / i;

    r = r + (double)i;
    r = r + (double)u;

    return r;
}

float test_float_int_mixed(void)
{
    float r = 1.5f;
    int i = -3;
    unsigned char uc = 250;

    r = r + i;
    r = r + uc;
    r = r * i;
    r = r / uc;

    r = r + (float)i;
    r = r + (float)uc;

    return r;
}

long double test_long_double_mixed(void)
{
    long double r = 1.2345L;
    int i = 7;
    long long ll = -9999LL;

    r = r + i;
    r = r + ll;
    r = r * i;
    r = r / ll;

    r = r + (long double)i;
    r = r + (long double)ll;

    return r;
}