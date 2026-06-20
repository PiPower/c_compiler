#include <stdio.h>
#include <stdint.h>
#define DEFINE_FUNC(name, expr) \
int name(int x) {               \
    int y = x * 2;              \
    y += 5;                     \
    return (expr);              \
}

DEFINE_FUNC(foo, y + 1)

static int failures = 0;

#define CHECK(expr)                                                     \
    do {                                                                \
        if (!(expr)) {                                                  \
            printf("FAIL: %s (line %d)\n", #expr, __LINE__);            \
            failures++;                                                 \
        }                                                               \
    } while (0)

/* ------------------------------------------------------------------ */
/* Basic calls                                                        */
/* ------------------------------------------------------------------ */

int f0(void)
{
    return 123;
}

int f1(int a)
{
    return a + 1;
}

int f_ptr(int* a)
{
    return *a  + 34;;
}

int f2(int a, int b)
{
    return a + b;
}

int f5(int a, int b, int c, int d, int e)
{
    return a + b + c + d + e;
}

/* ------------------------------------------------------------------ */
/* Many arguments                                                     */
/* ------------------------------------------------------------------ */

long long many_args(
    long long a1,
    long long a2,
    long long a3,
    long long a4,
    long long a5,
    long long a6,
    long long a7,
    long long a8,
    long long a9,
    long long a10)
{
    return a1 + a2 + a3 + a4 + a5 +
           a6 + a7 + a8 + a9 + a10;
}

/* ------------------------------------------------------------------ */
/* Pointer arguments                                                  */
/* ------------------------------------------------------------------ */

int sum_array(const int *arr, int n)
{
    int s = 0;
    int i;

    for (i = 0; i < n; i++)
        s += arr[i];

    return s;
}

/* ------------------------------------------------------------------ */
/* Recursion                                                          */
/* ------------------------------------------------------------------ */

int factorial(int n)
{
    if (n <= 1)
        return 1;

    return n * factorial(n - 1);
}

/* ------------------------------------------------------------------ */
/* Function pointers                                                  */
/* ------------------------------------------------------------------ */

int add(int a, int b)
{
    return a + b;
}

int sub(int a, int b)
{
    return a - b;
}

int call_binary(int (*fn)(int, int), int a, int b)
{
    return fn(a, b);
}

/* ------------------------------------------------------------------ */
/* Struct passing                                                     */
/* ------------------------------------------------------------------ */

struct Pair
{
    int x;
    long xd;
    int y;
};

struct Pair2
{
    int x;
    int y;
    int y2;
};
struct Pair make_pair(int x, int y, struct Pair2 p2, struct Pair p3)
{
    struct Pair p;
    p.x = x;
    p.y = p3.x;
    return p;
}

int pair_sum(struct Pair p)
{
    return p.x + p.y;
}

/* ------------------------------------------------------------------ */
/* Mixed argument types                                               */
/* ------------------------------------------------------------------ */

long long mixed(
    char a,
    short b,
    int c,
    long long d)
{
    return (long long)a + b + c + d;
}

/* ------------------------------------------------------------------ */
/* Nested calls                                                       */
/* ------------------------------------------------------------------ */

int inc(int x)
{
    return x + 1;
}

int triple(int x)
{
    return x * 3;
}

/* ------------------------------------------------------------------ */
/* Global state                                                       */
/* ------------------------------------------------------------------ */

static int counter = 0;

int bump(void)
{
    counter++;
    return counter;
}

/* ------------------------------------------------------------------ */
/* Main                                                               */
/* ------------------------------------------------------------------ */

int main(void)
{
    int arr[5];
    struct Pair p;

    CHECK(f0() == 123);
    CHECK(f1(41) == 42);
    CHECK(f2(20, 22) == 42);

    CHECK(f5(1, 2, 3, 4, 5) == 15);

    CHECK(
        many_args(
            1, 2, 3, 4, 5,
            6, 7, 8, 9, 10) == 55);

    //arr[0] = 1;
    //arr[1] = 2;
    //arr[2] = 3;
    //arr[3] = 4;
    //arr[4] = 5;

    CHECK(sum_array(arr, 5) == 15);

    CHECK(factorial(1) == 1);
    CHECK(factorial(5) == 120);

    CHECK(call_binary(add, 10, 20) == 30);
    CHECK(call_binary(sub, 10, 20) == -10);

    struct Pair2 p2 = {};
    p = make_pair(17, 25, p2, p);

    CHECK(p.x == 17);
    CHECK(p.y == 25);
    CHECK(pair_sum(p) == 42);

    CHECK(mixed(1, 2, 3, 4) == 10);

    CHECK(inc(triple(7)) == 22);

    CHECK(bump() == 1);
    CHECK(bump() == 2);
    CHECK(bump() == 3);

    CHECK(f2(f1(10), f1(20)) == 32);

    if (failures == 0)
    {
        printf("PASS\n");
        return 0;
    }

    printf("%d test(s) failed\n", failures);
    return 1;
}

