#define TEST(name) void name(void)
#define ASSERT_EQ(a, b)                                                        \
  do {                                                                         \
    if ((a) != (b)) {                                                          \
      fprintf(stderr, "FAIL %s:%d  %s == %s  (%d != %d)\n", __FILE__,          \
              __LINE__, #a, #b, (a), (b));                                     \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define ASSERT_STR_EQ(a, b)                                                    \
  do {                                                                         \
    if (strcmp(a, b)) {                                                        \
      fprintf(stderr, "FAIL %s:%d  \"%s\" != \"%s\"\n", __FILE__, __LINE__, a, \
              b);                                                              \
      exit(1);                                                                 \
    }                                                                          \
  } while (0)
#define RUN_TEST(t)                                                            \
  do {                                                                         \
    printf("Running " #t "... ");                                              \
    t();                                                                       \
    puts("OK");                                                                \
  } while (0)

int main(void) {
  RUN_TEST(test_something);
  RUN_TEST(test_another);
  puts("All tests passed!");
  return 0;
}
