typedef signed char prol;
typedef prol xd;

typedef long int yolo2;
typedef yolo2 prd;
typedef struct simple_struct
{
    int xasda : 2;
    int (*(*func_table[4][14])(char, ...))(double);
    int (*func_table2)[4][14];
    struct simple_struct_2
    {
        int x, p : 2;
        long int k;
    } pt[6], pk;
    int * const volatile x, p : 2;
    long int k;
} XDDDD;

XDDDD prkw;
//#include <stdio.h>


struct Outer {
    const volatile unsigned long id, gd, *ci;
    /* Bit-fields with mixed integer types */
    struct {
        unsigned int a:3;
        signed int b:5;
        unsigned long long c:9;
        _Bool flag:1;
    } bits;

    /* Anonymous union with nested structs */
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

    /* Array of pointers to functions returning pointers to functions */
    int (*(*func_table[4][14])(char, ...))(double);

    /* Pointer to array of 5 const pointers to volatile int */
    volatile int * const (*ptr_to_array)[5];

    /* Nested struct with flexible array member */
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

    /* Multi-dimensional array of function pointers */
    void (*handlers[3][2])(int, void *);

    /* Pointer to function returning pointer to array of 10 ints */
    int (*(*complex_func)(long, short))[10];

    /* Anonymous struct inside union inside struct */
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

    /* Self-referential pointer with restrict qualifier */
    struct Outer *restrict next;

    /* Array of structs containing unions with bit-fields */
    struct {
        union {
            struct {
                unsigned mode:2;
                unsigned state:3;
                unsigned error:1;
            };
            unsigned char packed;
        };
    } states[6];

    /* Flexible array member at end (C99) */
    unsigned char payload[];
} yolo;

int main()
{

}