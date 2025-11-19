#include <stdio.h>
#include <stdlib.h>

typedef enum { STATE_IDLE, STATE_RUNNING, STATE_PAUSED, STATE_STOPPED } State;

typedef enum { EVENT_START, EVENT_STOP, EVENT_PAUSE, EVENT_RESUME } Event;

State handle_idle(Event event) {
  switch (event) {
  case EVENT_START:
    return STATE_RUNNING;
  default:
    return STATE_IDLE;
  }
}

State handle_running(Event event) {
  switch (event) {
  case EVENT_PAUSE:
    return STATE_PAUSED;
  case EVENT_STOP:
    return STATE_STOPPED;
  default:
    return STATE_RUNNING;
  }
}

State handle_paused(Event event) {
  switch (event) {
  case EVENT_RESUME:
    return STATE_RUNNING;
  case EVENT_STOP:
    return STATE_STOPPED;
  default:
    return STATE_PAUSED;
  }
}

State handle_stopped(Event event) {
  switch (event) {
  case EVENT_START:
    return STATE_RUNNING;
  default:
    return STATE_STOPPED;
  }
}

typedef State (*StateHandler)(Event);

StateHandler state_handlers[] = {[STATE_IDLE] = handle_idle,
                                 [STATE_RUNNING] = handle_running,
                                 [STATE_PAUSED] = handle_paused,
                                 [STATE_STOPPED] = handle_stopped};

typedef struct {
  State current_state;
} StateMachine;

void sm_trigger_event(StateMachine *sm, Event event) {
  sm->current_state = state_handlers[sm->current_state](event);
}

const char *state_names[] = {[STATE_IDLE] = "IDLE",
                             [STATE_RUNNING] = "RUNNING",
                             [STATE_PAUSED] = "PAUSED",
                             [STATE_STOPPED] = "STOPPED"};

int main() {
  StateMachine sm = {STATE_IDLE};

  printf("Initial state: %s\n", state_names[sm.current_state]);

  sm_trigger_event(&sm, EVENT_START);
  printf("After START: %s\n", state_names[sm.current_state]);

  sm_trigger_event(&sm, EVENT_PAUSE);
  printf("After PAUSE: %s\n", state_names[sm.current_state]);

  sm_trigger_event(&sm, EVENT_RESUME);
  printf("After RESUME: %s\n", state_names[sm.current_state]);

  sm_trigger_event(&sm, EVENT_STOP);
  printf("After STOP: %s\n", state_names[sm.current_state]);

  return 0;
}
