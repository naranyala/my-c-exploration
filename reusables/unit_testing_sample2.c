/**
 * Simple unit testing framework
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TESTS 100

typedef struct {
  char name[64];
  void (*test_function)(void);
} TestCase;

typedef struct {
  TestCase tests[MAX_TESTS];
  int test_count;
  int passed;
  int failed;
} TestSuite;

void test_suite_init(TestSuite *suite) {
  suite->test_count = 0;
  suite->passed = 0;
  suite->failed = 0;
}

void test_suite_add(TestSuite *suite, const char *name,
                    void (*test_function)(void)) {
  if (suite->test_count < MAX_TESTS) {
    strncpy(suite->tests[suite->test_count].name, name,
            sizeof(suite->tests[0].name) - 1);
    suite->tests[suite->test_count].test_function = test_function;
    suite->test_count++;
  }
}

#define ASSERT(condition)                                                      \
  do {                                                                         \
    if (!(condition)) {                                                        \
      printf("FAIL: %s:%d: %s\n", __FILE__, __LINE__, #condition);             \
      return;                                                                  \
    }                                                                          \
  } while (0)

#define ASSERT_EQUAL(expected, actual) ASSERT((expected) == (actual))

#define ASSERT_STR_EQUAL(expected, actual)                                     \
  ASSERT(strcmp((expected), (actual)) == 0)

void test_suite_run(TestSuite *suite) {
  printf("Running %d tests...\n\n", suite->test_count);

  for (int i = 0; i < suite->test_count; i++) {
    printf("Running test: %s\n", suite->tests[i].name);

    suite->tests[i].test_function();

    printf("PASS: %s\n\n", suite->tests[i].name);
    suite->passed++;
  }

  printf("Results: %d passed, %d failed, %d total\n", suite->passed,
         suite->failed, suite->test_count);
}

// Example test functions
void test_addition(void) {
  int result = 2 + 2;
  ASSERT_EQUAL(4, result);
}

void test_string_operations(void) {
  char str[64];
  strcpy(str, "hello");
  ASSERT_STR_EQUAL("hello", str);
}

void test_memory_allocation(void) {
  int *ptr = malloc(sizeof(int));
  ASSERT(ptr != NULL);
  *ptr = 42;
  ASSERT_EQUAL(42, *ptr);
  free(ptr);
}

// Example usage
int main(void) {
  TestSuite suite;
  test_suite_init(&suite);

  test_suite_add(&suite, "test_addition", test_addition);
  test_suite_add(&suite, "test_string_operations", test_string_operations);
  test_suite_add(&suite, "test_memory_allocation", test_memory_allocation);

  test_suite_run(&suite);

  return 0;
}
