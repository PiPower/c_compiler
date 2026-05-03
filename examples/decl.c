/*
 * test_var_decl.c
 * C99 Variable Declaration Quality Test Suite
 */

/* =========================================================
 * 1. BASIC PRIMITIVE TYPES
 * ========================================================= */
typedef int (*(*func_table[4][14])(char, ...))(double);
typedef int *func_table1;

func_table ptr;

void test_basic_primitives(void) {
    char                a1 = 'A';
    char p;
    p = a1 = 123;
    short               a2 = -32000;
    int                 a3 = -2147483647;
    long                a4 = -2147483648L;
    long long           a5 = -9223372036854775807LL;

    unsigned char       b1 = 255u;
    unsigned short      b2 = 65535u;
    unsigned int        b3 = 4294967295u;
    unsigned long       b4 = 4294967295uL;
    unsigned long long  b5 = 18446744073709551615uLL;

    float               c1 = 3.14f;
    double              c2 = 3.141592653589793;
    long double         c3 = 3.141592653589793238L;

    _Bool               d1 = 1;
    _Bool               d2 = 0;
}

/* =========================================================
 * 2. QUALIFIERS: const, volatile, restrict
 * ========================================================= */

void test_qualifiers(void) {
    const int           ci  = 42;
    volatile int        vi  = 0;
    const volatile int  cvi = 99;

    const int          *pc  = &ci;
    int * const         cp  = (int *)&ci;
    const int * const   cpc = &ci;
    volatile int       *pv  = &vi;

    int * restrict      rp  = (int *)&vi;
}

/* =========================================================
 * 3. STORAGE-CLASS SPECIFIERS
 * ========================================================= */

static int s_static = 10;
int        s_global = 0;

void test_storage_class(void) {
    auto     int sc_auto   = 1;
    register int sc_reg    = 2;
    static   int sc_static = 3;
}

/* =========================================================
 * 4. POINTERS — MULTI-LEVEL, FUNCTION POINTERS
 * ========================================================= */

static int add(int x, int y) { return x + y; }

void test_pointers(void) {
    int   x  = 5;
    int  *p1 = &x;
    int **p2 = &p1;
    int ***p3 = &p2;

    const char *str = "hello";
    void       *np  = 0;

    int (*fp1)(int, int) = add;
    int (*fp2)(int, int);
    int (*fptable[4])(int, int);
    void *(*fp3)(void) = (void *(*)(void))0;
}

/* =========================================================
 * 5. ARRAYS — FIXED, VLA (C99), MULTIDIMENSIONAL
 * ========================================================= */

int g_arr[8] = {0, 1, 2, 3, 4, 5, 6, 7};
int g_implicit[] = {10, 20, 30};

void test_arrays(int n) {
    double fa[4] = {1.0, 2.0, 3.0, 4.0};

    int vla[n];

    int matrix[3][3] = {{1,0,0},{0,1,0},{0,0,1}};

    int (*pa)[4] = &(int[4]){1,2,3,4};

    const char *words[] = {"foo", "bar", "baz"};
}

/* =========================================================
 * 6. STRUCTS & UNIONS
 * ========================================================= */

struct Point {
    double x;
    double y;
};

struct Flags {
    unsigned int read  : 1;
    unsigned int write : 1;
    unsigned int exec  : 1;
    unsigned int       : 5;
};

struct Line {
    struct Point start;
    struct Point end;
};

union Data {
    unsigned char  bytes[4];
    unsigned int   word;
    float          real;
};

void test_structs(void) {
    struct Point p1 = {0.0, 0.0};
    struct Point p2 = {.x = 1.0, .y = 2.0};
    struct Flags fl = {.read = 1, .write = 1, .exec = 0};
    struct Line  ln = {.start = {0,0}, .end = {1,1}};
    union Data   d  = {.word = 0xDEADBEEFu};
}

/* =========================================================
 * 7. ENUMERATIONS
 * ========================================================= */

enum Color {
    COLOR_RED   = 0,
    COLOR_GREEN = 1,
    COLOR_BLUE  = 2
};

enum Direction {
    DIR_NORTH,
    DIR_EAST,
    DIR_SOUTH,
    DIR_WEST
};

void test_enums(void) {
    enum Color     c   = COLOR_RED;
    enum Direction d   = DIR_NORTH;
    int            raw = (int)COLOR_GREEN;
}

/* =========================================================
 * 8. TYPEDEF DECLARATIONS
 * ========================================================= */

typedef unsigned char  byte_t;
typedef int           *int_ptr_t;
typedef void         (*callback_t)(int);
typedef struct Point   Point2D;
typedef int            Matrix3x3[3][3];

void dummy_cb(int x) {}

void test_typedefs(void) {
    byte_t     b  = 0xABu;
    int_ptr_t  p  = (int *)&b;
    callback_t cb = dummy_cb;
    Point2D    pt = {3.0, 4.0};
    Matrix3x3  m  = {{1,0,0},{0,1,0},{0,0,1}};
}

/* =========================================================
 * 9. COMPOUND LITERALS (C99)
 * ========================================================= */

void test_compound_literals(void) {
    int          *pi  = &(int){42};
    struct Point *pp  = &(struct Point){.x = 5.0, .y = 6.0};
    int          *arr = (int[]){10, 20, 30, 40};
}

/* =========================================================
 * 10. MULTIPLE DECLARATORS IN ONE DECLARATION
 * ========================================================= */

void test_multi_declarator(void) {
    int a = 1, b = 2, c = 3;
    int *p = &a, *q = &b, r = 0;
    const double x = 1.0, y = 2.0;
}

/* =========================================================
 * 11. DECLARATIONS INSIDE for-LOOP INIT (C99)
 * ========================================================= */

void test_for_init_decl(void) {
    for (int i = 0; i < 10; i++) {}

    for (int i = 0, j = 10; i < j; i++, j--) {}
}

/* =========================================================
 * 12. MIXED DECLARATIONS AND CODE (C99)
 * ========================================================= */

void test_mixed_decl_code(void) {
    int x = 10;
    x += 5;
    int y = x * 2;

    {
        double inner = 3.14;
        inner *= 2.0;
        long z = (long)inner;
    }
}

/* =========================================================
 * 13. EDGE CASES
 * ========================================================= */

/* Tentative definitions */
int tentative;
int tentative = 0;

/* Array parameter decays to pointer */
void arr_param_decay(int arr[], int n) {}

/* Function with no params vs old-style unspecified */
void no_params(void);
void unspecified_params();

void test_edge_cases(void) {
    /* VLA sizeof — evaluated at runtime */
    int n = 5;
    int vla[n];
    long sz = (long)sizeof(vla);

    /* _Bool from expression */
    _Bool flag = (3 > 2);

    /* Chained pointer cast */
    unsigned int   ui = 0xDEADu;
    unsigned char *uc = (unsigned char *)&ui;

    /* Volatile through pointer */
    volatile int  vi = 0;
    int          *vp = (int *)&vi;
}

/* =========================================================
 * ENTRY POINT
 * ========================================================= */

int main(void) {
    test_basic_primitives();
    test_qualifiers();
    test_storage_class();
    test_pointers();
    test_arrays(6);
    test_structs();
    test_enums();
    test_typedefs();
    test_compound_literals();
    test_multi_declarator();
    test_for_init_decl();
    test_mixed_decl_code();
    test_edge_cases();
    return 0;
}