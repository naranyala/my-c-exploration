#define FSM_BEGIN(name)                                                        \
  void name##_run(int *state, int event) {                                     \
    switch (*state) {
#define STATE(s) case s:
#define ON(event, code)                                                        \
  if (event == event) {                                                        \
    code;                                                                      \
    break;                                                                     \
  }
#define GOTO(s)                                                                \
  do {                                                                         \
    *state = s;                                                                \
    return;                                                                    \
  } while (0)
#define FSM_END                                                                \
  default:;                                                                    \
    }                                                                          \
    }
