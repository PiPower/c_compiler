/* c99_preprocessor_if_suite.c
   Comprehensive C99 preprocessor conditional compilation test suite
*/

#pragma GCC warning "===== C99 PREPROCESSOR CONDITIONAL TEST SUITE START ====="

/* ====================================================== */
/* Section 1 — Basic #ifdef / #ifndef */
/* ====================================================== */

#define FEATURE_A

#ifdef FEATURE_C
#pragma GCC warning "[FAIL] #ifdef FEATURE_A -> true"
#else
#pragma GCC warning "[PASS] #ifdef FEATURE_A -> false"
#endif

#ifndef FEATURE_B
#pragma GCC warning "[PASS] #ifndef FEATURE_B -> true"
#elif FEATURE_B == 1
#pragma GCC warning "[FAIL] FEATURE_B == 1"
#else
#pragma GCC warning "[FAIL] FEATURE_B unexpected branch"
#endif


/* ====================================================== */
/* Section 2 — Basic #if constants */
/* ====================================================== */

#if 1
#pragma GCC warning "[PASS] Test 1: #if 1"
#else
#pragma GCC warning "[FAIL] Test 1"
#endif

#if 0
#pragma GCC warning "[FAIL] Test 2"
#else
#pragma GCC warning "[PASS] Test 2: #if 0"
#endif


/* ====================================================== */
/* Section 3 — Arithmetic and precedence */
/* ====================================================== */

#if 1 + 2 * 3 == 7
#pragma GCC warning "[PASS] Test 3: precedence 1 + 2 * 3"
#else
#pragma GCC warning "[FAIL] Test 3"
#endif

#if (10 - 4) / 2 == 3
#pragma GCC warning "[PASS] Test 4: parentheses"
#else
#pragma GCC warning "[FAIL] Test 4"
#endif


/* ====================================================== */
/* Section 4 — Macro expansion */
/* ====================================================== */

#define A 10
#define B 20

#if A + B == 30
#pragma GCC warning "[PASS] Test 5: macro expansion"
#else
#pragma GCC warning "[FAIL] Test 5"
#endif


/* ====================================================== */
/* Section 5 — Undefined macro behavior */
/* ====================================================== */

#if UNDEFINED_MACRO
#pragma GCC warning "[FAIL] Test 6"
#else
#pragma GCC warning "[PASS] Test 6: undefined macro -> 0"
#endif


/* ====================================================== */
/* Section 6 — defined() operator */
/* ====================================================== */

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


/* ====================================================== */
/* Section 7 — Logical operators */
/* ====================================================== */

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


/* ====================================================== */
/* Section 8 — Bitwise operators */
/* ====================================================== */

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


/* ====================================================== */
/* Section 9 — Nested conditionals */
/* ====================================================== */

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


/* ====================================================== */
/* Section 10 — #elif chain */
/* ====================================================== */

#define VALUE_TEST 3

#if VALUE_TEST == 1
#pragma GCC warning "[FAIL] Test 15"
#elif VALUE_TEST == 2
#pragma GCC warning "[FAIL] Test 15"
#elif VALUE_TEST == 3
#pragma GCC warning "[PASS] Test 15: elif chain"
#else
#pragma GCC warning "[FAIL] Test 15"
#endif


/* ====================================================== */
/* Section 11 — Macro expressions */
/* ====================================================== */

#define EXPR 2 + 3

#if EXPR * 2 == 10
#pragma GCC warning "[PASS] Test 16: macro expression"
#else
#pragma GCC warning "[PASS] Test 16 macro subsitution"
#endif


/* ====================================================== */
/* Section 12 — Complex expression */
/* ====================================================== */

#define CA 5
#define CB 3

#if (CA * CB > 10) && defined(CA) && !defined(CC)
#pragma GCC warning "[PASS] Test 17: complex expression"
#else
#pragma GCC warning "[FAIL] Test 17"
#endif


/* ====================================================== */
/* Section 13 — Character constants */
/* ====================================================== */

#if 'A' == 65
#pragma GCC warning "[PASS] Test 18: character constant"
#else
#pragma GCC warning "[FAIL] Test 18"
#endif


/* ====================================================== */
/* Section 14 — Large integers */
/* ====================================================== */

#if 2147483647 > 0
#pragma GCC warning "[PASS] Test 19: large integer"
#else
#pragma GCC warning "[FAIL] Test 19"
#endif


/* ====================================================== */
/* Section 15 — Skipped code */
/* ====================================================== */

#if 0

THIS_TEXT_SHOULD_BE_IGNORED
#if RANDOM
#endif
!@#$%^&*() tokens

#endif

#pragma GCC warning "[PASS] Test 20: dead code skipped"


/* ====================================================== */
/* Section 16 — Deep nesting */
/* ====================================================== */

#define DN 1

#if DN
#if DN
#if DN
#pragma GCC warning "[PASS] Test 21: deep nesting"
#endif
#endif
#endif


/* ====================================================== */
/* Section 17 — Shift precedence */
/* ====================================================== */

#if 1 + 2 << 3 == 24
#pragma GCC warning "[PASS] Test 22: shift precedence"
#else
#pragma GCC warning "[FAIL] Test 22"
#endif


/* ====================================================== */
/* Section 18 — Unary operators */
/* ====================================================== */

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


/* ====================================================== */
/* Section 19 — Ternary operator */
/* ====================================================== */

#if (1 ? 2 : 3) == 2
#pragma GCC warning "[PASS] Test 25: ternary"
#else
#pragma GCC warning "[FAIL] Test 25"
#endif


/* ====================================================== */
/* Section 20 — defined() without expansion */
/* ====================================================== */

#define FOO BAR

#if defined(FOO)
#pragma GCC warning "[PASS] Test 26: defined without expansion"
#else
#pragma GCC warning "[FAIL] Test 26"
#endif


#pragma GCC warning "===== C99 PREPROCESSOR CONDITIONAL TEST SUITE END ====="