/* ergo_test.h - Micro Unit Testing Framework */
#ifndef ERGO_TEST_H
#define ERGO_TEST_H

#include <stdio.h>

// Globals to track stats
extern int ergo_tests_run;
extern int ergo_tests_failed;

// Assertion Macros
#define ergo_assert(test, msg) do { \
    if (!(test)) { \
        printf("\x1b[31m[FAIL]\x1b[0m %s:%d: %s\n", __FILE__, __LINE__, msg); \
        ergo_tests_failed++; \
        return; \
    } \
} while (0)

#define ergo_assert_eq(a, b) ergo_assert((a) == (b), "Values are not equal")

// Test Runner Macro
#define ergo_run_test(test_func) do { \
    printf("Running %s... ", #test_func); \
    int failed_before = ergo_tests_failed; \
    test_func(); \
    if (ergo_tests_failed == failed_before) { \
        printf("\x1b[32m[PASS]\x1b[0m\n"); \
    } \
    ergo_tests_run++; \
} while (0)

// Summary Printer
void ergo_test_summary();

#endif // ERGO_TEST_H

#ifdef ERGO_IMPLEMENTATION
int ergo_tests_run = 0;
int ergo_tests_failed = 0;

void ergo_test_summary() {
    printf("------------------------------\n");
    if (ergo_tests_failed == 0) {
        printf("\x1b[32mALL TESTS PASSED (%d run)\x1b[0m\n", ergo_tests_run);
    } else {
        printf("\x1b[31mFAILURES: %d / %d tests run\x1b[0m\n", ergo_tests_failed, ergo_tests_run);
    }
}
#endif
