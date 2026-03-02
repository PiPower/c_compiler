#include <stdio.h>
#define MAX 100

typedef struct Point { int x, y; } Point_t;

int main(void) {
    int \u9323 = 939, \U85483922 = 395995l;
    char \u93F3\UAAFFf293sdsa3 = '\x34';
    int a = 5, b = 10;
    float f = 3.14, g = 2.71;
    double d = 1e3;
    long l = 1234567890L;
    char c = 'Z';
    _Bool flag = 1;
    const int ci = 42;
    volatile int vi = 0;
    if (a < b && b > 0 || !flag) {
        a += 1; b -= 1; c++; d--;
    } else {
        a *= 2; b /= 2; f *= g; g /= f;
    }
    switch(a) {
        case 1: break;
        default: ; 
    }
    while(a < MAX) {
        a <<= 1;
        b >>= 1;
    }
    Point_t p = { .x = a, .y = b };
    Point_t* ptr = &p;
    ptr->x = 0;
    // This is a comment with tricky symbols: ++ -- && || == !=
    return 0;
}