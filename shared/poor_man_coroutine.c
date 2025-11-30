#define CO_BEGIN(state)                                                        \
  switch (state) {                                                             \
  case 0:
#define CO_YIELD(state, n)                                                     \
  do {                                                                         \
    state = n;                                                                 \
    return;                                                                    \
  case n:;                                                                     \
  } while (0)
#define CO_END(state) }

void generator(int *state) {
  static int i;
  CO_BEGIN(*state);
  for (i = 0; i < 100; i++) {
    CO_YIELD(*state, __LINE__);
    printf("%d\n", i);
  }
  CO_END(*state);
}
