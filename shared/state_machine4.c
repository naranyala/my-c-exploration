/**
 * Finite State Machine implementation
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum {
  STATE_IDLE,
  STATE_ACTIVE,
  STATE_PAUSED,
  STATE_STOPPED,
  STATE_COUNT
} State;

typedef enum {
  EVENT_START,
  EVENT_PAUSE,
  EVENT_RESUME,
  EVENT_STOP,
  EVENT_RESET,
  EVENT_COUNT
} Event;

typedef struct {
  State current_state;
  void (*on_enter[STATE_COUNT])(void);
  void (*on_exit[STATE_COUNT])(void);
  State transition_table[STATE_COUNT][EVENT_COUNT];
} StateMachine;

// State action functions
void on_enter_idle(void) { printf("Entering IDLE state\n"); }
void on_exit_idle(void) { printf("Exiting IDLE state\n"); }

void on_enter_active(void) { printf("Entering ACTIVE state\n"); }
void on_exit_active(void) { printf("Exiting ACTIVE state\n"); }

void on_enter_paused(void) { printf("Entering PAUSED state\n"); }
void on_exit_paused(void) { printf("Exiting PAUSED state\n"); }

void on_enter_stopped(void) { printf("Entering STOPPED state\n"); }
void on_exit_stopped(void) { printf("Exiting STOPPED state\n"); }

StateMachine *state_machine_create(State initial_state) {
  StateMachine *sm = malloc(sizeof(StateMachine));
  if (!sm)
    return NULL;

  sm->current_state = initial_state;

  // Initialize transition table
  for (int i = 0; i < STATE_COUNT; i++) {
    for (int j = 0; j < EVENT_COUNT; j++) {
      sm->transition_table[i][j] = i; // Default: stay in current state
    }
  }

  // Define valid transitions
  sm->transition_table[STATE_IDLE][EVENT_START] = STATE_ACTIVE;
  sm->transition_table[STATE_ACTIVE][EVENT_PAUSE] = STATE_PAUSED;
  sm->transition_table[STATE_ACTIVE][EVENT_STOP] = STATE_STOPPED;
  sm->transition_table[STATE_PAUSED][EVENT_RESUME] = STATE_ACTIVE;
  sm->transition_table[STATE_PAUSED][EVENT_STOP] = STATE_STOPPED;
  sm->transition_table[STATE_STOPPED][EVENT_RESET] = STATE_IDLE;

  // Set state action callbacks
  sm->on_enter[STATE_IDLE] = on_enter_idle;
  sm->on_exit[STATE_IDLE] = on_exit_idle;
  sm->on_enter[STATE_ACTIVE] = on_enter_active;
  sm->on_exit[STATE_ACTIVE] = on_exit_active;
  sm->on_enter[STATE_PAUSED] = on_enter_paused;
  sm->on_exit[STATE_PAUSED] = on_exit_paused;
  sm->on_enter[STATE_STOPPED] = on_enter_stopped;
  sm->on_exit[STATE_STOPPED] = on_exit_stopped;

  return sm;
}

bool state_machine_handle_event(StateMachine *sm, Event event) {
  State current = sm->current_state;
  State next = sm->transition_table[current][event];

  if (current == next) {
    printf("Invalid transition from state %d with event %d\n", current, event);
    return false;
  }

  // Execute exit action for current state
  if (sm->on_exit[current]) {
    sm->on_exit[current]();
  }

  // Change state
  sm->current_state = next;

  // Execute enter action for new state
  if (sm->on_enter[next]) {
    sm->on_enter[next]();
  }

  printf("Transition: %d -> %d (Event: %d)\n", current, next, event);
  return true;
}

void state_machine_destroy(StateMachine *sm) { free(sm); }

// Example usage
int main(void) {
  StateMachine *sm = state_machine_create(STATE_IDLE);

  state_machine_handle_event(sm, EVENT_START);
  state_machine_handle_event(sm, EVENT_PAUSE);
  state_machine_handle_event(sm, EVENT_RESUME);
  state_machine_handle_event(sm, EVENT_STOP);
  state_machine_handle_event(sm, EVENT_RESET);

  state_machine_destroy(sm);
  return 0;
}
