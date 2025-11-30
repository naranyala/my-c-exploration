
// statemachine.h
#ifndef STATEMACHINE_H
#define STATEMACHINE_H

typedef enum { STATE_INIT, STATE_RUNNING, STATE_STOPPED } State;

typedef struct {
  State current;
  void (*on_enter[3])(void);
  void (*on_exit[3])(void);
} StateMachine;

static inline void sm_transition(StateMachine *sm, State next) {
  if (sm->on_exit[sm->current])
    sm->on_exit[sm->current]();
  sm->current = next;
  if (sm->on_enter[next])
    sm->on_enter[next]();
}

#endif
