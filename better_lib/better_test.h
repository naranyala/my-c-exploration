/* better_test.h */
#ifndef BETTER_TEST_H
#define BETTER_TEST_H

#include <stdio.h>

// Global stats
extern int bt_tests_run;
extern int bt_tests_failed;

// Macros
#define BT_ASSERT(cond) do { \
    if (!(cond)) { \
        printf("\x1b[31m[FAIL]\x1b[0m %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        bt_tests_failed++; \
        return; \
    } \
} while(0)

#define BT_TEST(name) void name(void)

#define BT_RUN(test_func) do { \
    int prev_fails = bt_tests_failed; \
    test_func(); \
    bt_tests_run++; \
    if (bt_tests_failed == prev_fails) { \
        printf("\x1b[32m[PASS]\x1b[0m %s\n", #test_func); \
    } \
} while(0)

void bt_report(void);

#endif

/* IMPLEMENTATION */
#ifdef BETTER_TEST_IMPLEMENTATION
int bt_tests_run = 0;
int bt_tests_failed = 0;

void bt_report(void) {
    printf("--------------------------------------\n");
    printf("Tests Run: %d, Failed: %d\n", bt_tests_run, bt_tests_failed);
    if (bt_tests_failed == 0) printf("\x1b[32mALL TESTS PASSED\x1b[0m\n");
    else printf("\x1b[31mSOME TESTS FAILED\x1b[0m\n");
}
#endif
