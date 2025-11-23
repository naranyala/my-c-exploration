#define BETTER_TEST_IMPLEMENTATION
#include "better_test.h"

// 1. Define functions to test
int add(int a, int b) { return a + b; }

// 2. Write tests
BT_TEST(test_addition) {
    BT_ASSERT(add(2, 2) == 4);
    BT_ASSERT(add(-1, 1) == 0);
}

BT_TEST(test_failure_example) {
    BT_ASSERT(add(2, 2) == 5); // This will fail
}

// 3. Run them
int main() {
    BT_RUN(test_addition);
    BT_RUN(test_failure_example);
    
    bt_report();
    return bt_tests_failed == 0 ? 0 : 1;
}
