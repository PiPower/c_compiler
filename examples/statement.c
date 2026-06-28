/* test_statements.c
 * Comprehensive C99 statement parser test.
 * Covers every statement form defined in ISO/IEC 9899:1999 §6.8.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
 
/* ── helpers ────────────────────────────────────────────────────── */
static int g = 0;
static void side_effect(void) { g++; }
 
/* ================================================================
 * 1. EXPRESSION STATEMENTS
 * ================================================================ */
void test_expression_statements(void)
{
    int x;
 
    /* simple assignment */
    x = 42;
 
    /* compound assignment */
    x += 1;
    x -= 1;
    x *= 2;
    x /= 2;
    x %= 7;
    x &= 0xFF;
    x |= 0x01;
    x ^= 0x0F;
    x <<= 2;
    x >>= 1;
 
    /* increment / decrement */
    x++;
    ++x;
    x--;
    --x;
    /* comma operator as expression statement */
    x = (1, 2, 3);
 
    /* void expression (function call) */
    side_effect();
 
    /* null statement */
    ;
 
    (void)x;
}
 
/* ================================================================
 * 2. COMPOUND (BLOCK) STATEMENTS
 * ================================================================ */
void test_compound_statements(void)
{
    /* empty block */
    {}
 
    /* nested blocks with shadowing */
    {
        int a = 1;
        {
            int a = 2;          /* shadows outer a */
            {
                int a = 3;
                (void)a;
            }
            (void)a;
        }
        (void)a;
    }
 
    /* C99: mixed declarations and statements inside a block */
    {
        int i = 0;
        i++;
        int j = i * 2;  /* declaration after statement — C99 only */
        (void)j;
    }
}
 
/* ================================================================
 * 3. SELECTION STATEMENTS  (if / if-else / switch)
 * ================================================================ */
void test_selection_statements(int n)
{
    double p = 0;
    /* 3a. plain if */
    if (n > 0)
        p += 1.0;
 
    /* 3b. if-else */
    if (n >= 0)
        p += 2.0;
    else
        p += 3.0;
 
    int ssd = 11;
    /* 3c. if-else-if chain (dangling-else resolved to nearest if) */
    if (n < 0)
        ssd -= 1;
    else if (n == 0)
        ssd *= 32;
    else
        ssd /= 2;
 
    /* 3d. nested if — classic dangling-else */
    if (n <= 0)
        if (n > 100)
            ssd += 9;
        else            /* binds to inner if */
            ssd -= 21;
 
    /* 3e. switch with fall-through, break, default */
    switch (n) {
        {
            int ppr = n * 23;
        }
    case 0:
        side_effect();
        break;
    case 1:
    case 2:             /* fall-through */
        side_effect();
        break;
    case 3: {           /* compound statement as case body */
        int local = n * 2;
        (void)local;
        break;
    }
    default:
        side_effect();
        break;
    }
 
    /* 3f. switch with no break (intentional fall-through) */
    switch (n % 3) {
    case 0: side_effect(); /* FALLTHROUGH */
    case 1: side_effect(); /* FALLTHROUGH */
    case 2: side_effect(); break;
    }
 
    /* 3g. switch on char */
    char c = (char)n;
    switch (c) {
    case 'a': case 'e': case 'i': case 'o': case 'u':
        side_effect(); break;
    default:
        break;
    }
}
 
/* ================================================================
 * 4. ITERATION STATEMENTS  (while / do-while / for)
 * ================================================================ */
void test_iteration_statements(void)
{
    int i, sum;

    /* 4a. while */
    i = 0;
    while (i < 10)
        i += 2;

    /* 4b. while with compound body */
    i = 0;
    while (i < 5) {
        sum += i;
        i++;
    }

    /* 4c. do-while */
    i = 0;
    do {
        i++;
    } while (i < 10);

    /* 4d. do-while runs at least once */
    i = 0;
    do {
        i += 1;
    } while (0);

    /* 4e. for — all three clauses */
    sum = 0;
    for (i = 0; i < 10; i++)
        sum += i;

    /* 4f. for — C99 declaration in init */
    for (int k = 0; k < 5; k++)
        sum += k;

    /* 4g. for — empty clauses (infinite loop, broken by break) */
    for (;;) {
        sum += 1;
        break;
    }

    /* 4h. for — missing update */
    for (i = 0; i < 3;) {
        i++;
        sum += i;
    }

    /* 4i. for — missing condition (treated as 1) */
    for (i = 0;; i++) {
        sum += i;
        if (i >= 3)
            break;
    }

    /* 4j. nested loops with labeled break/continue */
    sum = 0;
    for (int r = 0; r < 4; r++) {
        for (int col = 0; col < 4; col++) {
            if (col == 2) continue;
            if (r == 3) break;
            sum += r * col;
        }
    }
    (void)sum;

    /* 4k. while with break */
    i = 0;
    while (1) {
        sum += i;
        if (i++ > 5)
            break;
    }

    /* 4l. while with continue */
    i = 0;
    sum = 0;
    while (i < 10) {
        i++;
        if (i % 2 == 0)
            continue;
        sum += i;
    }
    (void)sum;
}
/* ================================================================
 * 5. JUMP STATEMENTS  (goto / continue / break / return)
 * ================================================================ */
void test_jump_statements(int n)
{
    int x = 0;
 
    /* 5a. goto forward */
    if (n < 0) goto negative;
    x = 1;
    goto done;
 
negative:
    x = -1;
 
done:
    (void)x;
 
    /* 5b. goto used for loop-like construct */
    {
        int count = 0;
    loop_start:
        if (count < 5) { count++; goto loop_start; }
    }
 
    /* 5c. return void */
    if (n == 999) return;
 
    /* 5d. break and continue tested in iteration section */
}
 
int test_return_value(int n)
{
    /* 5e. return with expression */
    if (n > 0)  return  n;
    if (n < 0)  return -n;
    return 0;
}
 
/* ================================================================
 * 6. LABELED STATEMENTS
 * ================================================================ */
void test_labeled_statements(int n)
{
    /* plain label */
    start:
        side_effect();
 
    /* label on null statement */
    end:
        ;
 
    /* label on a block */
    block_label:
    {
        int tmp = n;
        (void)tmp;
    }
 
    /* ensure labels are referenced to avoid warnings */
    if (n < 0) goto start;
    if (n > 0) goto end;
    goto block_label;
}
 
/* ================================================================
 * 7. DECLARATION STATEMENTS (C99: mixed with code)
 * ================================================================ */
void test_declaration_statements(void)
{
    /* VLA — C99 */
    int n = 5;
    //int vla[n];
    //for (int i = 0; i < n; i++) vla[i] = i;
    //(void)vla[0];
 
    /* const-qualified */
    const int CI = 42;
    (void)CI;
 
    /* restrict pointer — C99 */
    int arr[8];
    int *restrict rp = arr;
    *rp = 1;
 
    /* inline declaration in for, while condition (C99) */
    for (int i = 0, j = 10; i < j; i++, j--)
        side_effect();
 
    /* _Bool */
    _Bool flag = 1;
    if (flag) side_effect();
 
    /* compound literal — C99 */
    int *p = (int[]){1, 2, 3};
    (void)p;
 
    /* designated initializer — C99 */
    int d[5] = { [0]=1, [4]=5 };
    (void)d[4];
}
 
/* ================================================================
 * 8. EDGE / STRESS CASES
 * ================================================================ */
void test_edge_cases(void)
{
    /* deeply nested if-else */
    int x = g;
    if (x==0) { if (x==0) { if (x==0) { if (x==0) side_effect(); } } }
 
    /* switch inside while inside if */
    if (x >= 0) {
        int i = 0;
        while (i < 3) {
            switch (i) {
            case 0: break;
            case 1: i++; continue;  /* continue targets while */
            default: break;
            }
            i++;
        }
    }
 
    /* comma expression as for update */
    for (int a = 0, b = 10; a < b; a++, b--)
        ;
 
    /* nested ternary (expression statement) */
    int r = (x > 0) ? ((x > 10) ? 2 : 1) : 0;
    (void)r;
 
    /* empty switch */
    switch (x) { }
 
    /* switch with only default */
    switch (x) { default: break; }
 
    /* label immediately before closing brace (needs null stmt) */
    {
        goto skip;
    skip:
        ;
    }
}
 
/* ================================================================
 * 9. ENTRY POINT
 * ================================================================ */
int main(void)
{
    test_expression_statements();
    test_compound_statements();
    test_selection_statements(0);
    test_selection_statements(1);
    test_selection_statements(-1);
    test_selection_statements(42);
    test_iteration_statements();
    test_jump_statements(1);
    test_jump_statements(-1);
    test_jump_statements(0);
    test_labeled_statements(0);
    test_declaration_statements();
    test_edge_cases();
 
    int v = test_return_value(5);
    v = test_return_value(-3);
    v = test_return_value(0);
    (void)v;
 
    //printf("All statement parser tests passed.\n");
    return 0;
}