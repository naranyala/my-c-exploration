
#include <stdio.h>
#include <assert.h>

typedef void (*TestFunction)();

typedef struct {
    const char *name;
    TestFunction function;
} TestCase;

// Run a test case
void runTest(TestCase test) {
    printf("Running test: %s\n", test.name);
    test.function();
    printf("Test passed: %s\n", test.name);
}

// Example test function
void testExample() {
    assert(1 == 1);
}

// Define test cases
TestCase testCases[] = {
    {"Example Test", testExample},
    {NULL, NULL} // End of test cases
};

// Run all tests
void runAllTests() {
    for (int i = 0; testCases[i].name != NULL; i++) {
        runTest(testCases[i]);
    }
}
