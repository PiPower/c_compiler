/* preprocessor_if_tests.c
   C99 #if test suite using pragma warnings
*/

#pragma GCC warning "===== C99 PREPROCESSOR #IF TEST SUITE START ====="

/* -------------------------------------------------- */
/* Test 1 */
#if 1
#pragma GCC warning "[PASS] Test 1: #if 1"
#else
#pragma GCC warning "[FAIL] Test 1"
#endif

/* Test 2 */
#if 0
#pragma GCC warning "[FAIL] Test 2"
#else
#pragma GCC warning "[PASS] Test 2: #if 0"
#endif

/* -------------------------------------------------- */
/* Test 3 */
#if 1 + 2 * 3 == 7
#pragma GCC warning "[PASS] Test 3: precedence 1 + 2 * 3"
#else
#pragma GCC warning "[FAIL] Test 3"
#endif

/* Test 4 */
#if (10 - 4) / 2 == 3
#pragma GCC warning "[PASS] Test 4: parentheses"
#else
#pragma GCC warning "[FAIL] Test 4"
#endif

/* -------------------------------------------------- */
/* Test 5 */
#define A 10
#define B 20

#if A + B == 30
#pragma GCC warning "[PASS] Test 5: macro expansion"
#else
#pragma GCC warning "[FAIL] Test 5"
#endif

/* -------------------------------------------------- */
/* Test 6 */
#if UNDEFINED_MACRO
#pragma GCC warning "[FAIL] Test 6"
#else
#pragma GCC warning "[PASS] Test 6: undefined macro -> 0"
#endif

/* -------------------------------------------------- */
/* Test 7 */
#define X 1

#if defined(X)
#pragma GCC warning "[PASS] Test 7: defined(X)"
#else
#pragma GCC warning "[FAIL] Test 7"
#endif

#if defined(Y)
#pragma GCC warning "[FAIL] Test 8"
#else
#pragma GCC warning "[PASS] Test 8: defined(Y) false"
#endif

#if defined X
#pragma GCC warning "[PASS] Test 9: defined X syntax"
#else
#pragma GCC warning "[FAIL] Test 9"
#endif

/* -------------------------------------------------- */
/* Test 10 */
#define L1 1
#define L0 0

#if L1 && !L0
#pragma GCC warning "[PASS] Test 10: logical AND/NOT"
#else
#pragma GCC warning "[FAIL] Test 10"
#endif

#if L1 || L0
#pragma GCC warning "[PASS] Test 11: logical OR"
#else
#pragma GCC warning "[FAIL] Test 11"
#endif

/* -------------------------------------------------- */
/* Test 12 */
#if (4 & 1) == 0
#pragma GCC warning "[PASS] Test 12: bitwise AND"
#else
#pragma GCC warning "[FAIL] Test 12"
#endif

#if (8 >> 2) == 2
#pragma GCC warning "[PASS] Test 13: shift operator"
#else
#pragma GCC warning "[FAIL] Test 13"
#endif

/* -------------------------------------------------- */
/* Test 14 */
#define N1 1
#define N2 2

#if N1
    #if N2 == 2
        #pragma GCC warning "[PASS] Test 14: nested #if"
    #else
        #pragma GCC warning "[FAIL] Test 14"
    #endif
#else
#pragma GCC warning "[FAIL] Test 14"
#endif

/* -------------------------------------------------- */
/* Test 15 */
#define VALUE 3

#if VALUE == 1
#pragma GCC warning "[FAIL] Test 15"
#elif VALUE == 2
#pragma GCC warning "[FAIL] Test 15"
#elif VALUE == 3
#pragma GCC warning "[PASS] Test 15: elif chain"
#else
#pragma GCC warning "[FAIL] Test 15"
#endif

/* -------------------------------------------------- */
/* Test 16 */
#define EXPR 2 + 3

#if EXPR * 2 == 10
#pragma GCC warning "[PASS] Test 16: macro expression"
#else
#pragma GCC warning "[FAIL] Test 16"
#endif

/* -------------------------------------------------- */
/* Test 17 */
#define CA 5
#define CB 3

#if (CA * CB > 10) && defined(CA) && !defined(CC)
#pragma GCC warning "[PASS] Test 17: complex expression"
#else
#pragma GCC warning "[FAIL] Test 17"
#endif

/* -------------------------------------------------- */
/* Test 18 */
#if 'A' == 65
#pragma GCC warning "[PASS] Test 18: character constant"
#else
#pragma GCC warning "[FAIL] Test 18"
#endif

/* -------------------------------------------------- */
/* Test 19 */
#if 2147483647 > 0
#pragma GCC warning "[PASS] Test 19: large integer"
#else
#pragma GCC warning "[FAIL] Test 19"
#endif

/* -------------------------------------------------- */
/* Test 20 */
#if 0

THIS_TEXT_SHOULD_BE_IGNORED
#if RANDOM
#endif
!@#$%^&*() tokens

#endif

#pragma GCC warning "[PASS] Test 20: dead code skipped"

/* -------------------------------------------------- */
/* Test 21 */
#define DN 1

#if DN
#if DN
#if DN
#pragma GCC warning "[PASS] Test 21: deep nesting"
#endif
#endif
#endif

/* -------------------------------------------------- */
/* Test 22 */
#if 1 + 2 << 3 == 24
#pragma GCC warning "[PASS] Test 22: shift precedence"
#else
#pragma GCC warning "[FAIL] Test 22"
#endif

/* -------------------------------------------------- */
/* Test 23 */
#if -5 < 0
#pragma GCC warning "[PASS] Test 23: unary minus"
#else
#pragma GCC warning "[FAIL] Test 23"
#endif

#if ~0 == -1
#pragma GCC warning "[PASS] Test 24: bitwise NOT"
#else
#pragma GCC warning "[FAIL] Test 24"
#endif

/* -------------------------------------------------- */
/* Test 24 */
#if (1 ? 2 : 3) == 2
#pragma GCC warning "[PASS] Test 25: ternary"
#else
#pragma GCC warning "[FAIL] Test 25"
#endif

/* -------------------------------------------------- */
/* Test 25 */
#define FOO BAR

#if defined(FOO)
#pragma GCC warning "[PASS] Test 26: defined without expansion"
#else
#pragma GCC warning "[FAIL] Test 26"
#endif

#pragma GCC warning "===== C99 PREPROCESSOR #IF TEST SUITE END ====="