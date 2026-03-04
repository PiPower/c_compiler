/*
 * lexer_test.c
 * C99 Lexer Torture Test File
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define PI 3.141592653589793
#define SQUARE(x) ((x) * (x))
#define STR(x) #x
#define CONCAT(a,b) a##b
#define MULTILINE_MACRO(a, b) \
    ((a) > (b) ? (a) : (b))

// Single-line comment
/* Multi-line
   comment test */

typedef unsigned long ulong_t;

enum Color {
    RED = 1,
    GREEN,
    BLUE = 10,
};

struct Point {
    int x;
    int y;
};

union Data {
    int i;
    float f;
    char str[20];
};

static inline int add(int a, int b) {
    return a + b;
}

int global_var = 42;
const volatile int cv_var = 100;

void function_pointer_test(void) {
    int (*func_ptr)(int, int) = add;
    int result = func_ptr(3, 4);
    printf("Result: %d\n", result);
}

int main(void) {
    // Integer literals
    int dec = 123;
    int oct = 0123;
    int hex = 0x1A3F;
    unsigned int u = 123u;
    long l = 1234567890L;
    long long ll = 123456789012345LL;

    // Floating point literals
    float f1 = 3.14f;
    double d1 = 2.71828;
    double d2 = 1.23e-4;
    double d3 = 0x1.1p+2;  // hex float (C99)

    // Character literals
    char c1 = 'a';
    char c2 = '\n';
    char c3 = '\x41';

    // String literals
    char str1[] = "Hello, World!";
    char str2[] = "Escaped chars: \t \n \" \\";
    char str3[] = "Concatenated "
                  "string literal";

    struct Point p = { .x = 10, .y = 20 };
    union Data data;
    data.f = 3.14f;

    int arr[5] = { [0] = 1, [3] = 4 };
    int matrix[2][2] = {{1,2},{3,4}};

    // Operators
    int a = 5, b = 2;
    int arithmetic = a + b - a * b / b % a;
    int logical = (a > b) && (b < 10) || !(a == b);
    int bitwise = (a & b) | (a ^ b) << 1 >> 1;
    int ternary = (a > b) ? a : b;

    // Control flow
    if (a > 0) {
        for (int i = 0; i < 5; ++i) {
            printf("i = %d\n", i);
        }
    } else if (a == 0) {
        printf("Zero\n");
    } else {
        printf("Negative\n");
    }

    switch (a) {
        case 1:
            break;
        case 5:
            goto label_test;
        default:
            break;
    }

label_test:
    while (b--) {
        continue;
    }

    do {
        a--;
    } while (a > 0);

    // Sizeof and type casting
    size_t size = sizeof(struct Point);
    double casted = (double)a;

    // Restrict keyword (C99)
    int x = 10, y = 20;
    int *restrict rx = &x;
    int *restrict ry = &y;
    *rx += *ry;

    printf("Done.\n");
    return EXIT_SUCCESS;
}