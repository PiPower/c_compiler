#include <stdio.h>
typedef signed char prol;
typedef prol xd;

typedef long int yolo2;
typedef yolo2 prd;

typedef struct simple_struct
{
    int xasda;  // was :2

    int (*(*func_table[4][14])(char, ...))(double);
    int (*func_table2)[4][14];

    struct simple_struct_2
    {
        int x;
        int p;   // was :2
        long int k;
    } pt[6], pk;

    int * const volatile x;
    int p;  // was :2

    long int k;
} XDDDD;

XDDDD prkw;

struct Outer {
    const volatile unsigned long id, gd, *ci;

    /* Bit-fields removed */
    struct {
        unsigned int a;
        signed int b;
        unsigned long long c;
        _Bool flag;
    } bits;

    union {
        struct {
            int x;
            int y;
        };

        struct {
            double real;
            double imag;
        } complex;

        unsigned char raw[16];
    };

    int (*(*func_table[4][14])(char, ...))(double);

    volatile int * const (*ptr_to_array)[5];

    struct Nested {
        unsigned long len;

        union {
            char str[32];
            struct {
                unsigned short type;
                unsigned short flags;
                unsigned int data[];
            };
        };

        struct InnerMost {
            int (*callback)(struct Nested *self, void *ctx);
            void *ctx;
        } inner[2];
    } nested;

    void (*handlers[3][2])(int, void *);

    int (*(*complex_func)(long, short))[10];

    union {
        struct {
            unsigned char r, g, b, a;
        } color;

        unsigned int rgba;

        struct {
            unsigned short lo;
            unsigned short hi;
        };
    } pixel;

    struct Outer *restrict next;

    struct {
        union {
            struct {
                unsigned mode;
                unsigned state;
                unsigned error;
            };
            unsigned char packed;
        };
    } states[6];

    unsigned char payload[];
} yolo;

int main()
{
}