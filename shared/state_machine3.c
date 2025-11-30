
#include <stdio.h>

typedef enum {
    STATE_IDLE,
    STATE_RUNNING,
    STATE_PAUSED,
    STATE_EXIT
} State;

typedef struct {
    State currentState;
} StateMachine;

// Initialize the state machine
void initializeStateMachine(StateMachine *sm) {
    sm->currentState = STATE_IDLE;
}

// Transition to a new state
void transitionState(StateMachine *sm, State newState) {
    sm->currentState = newState;
    switch (sm->currentState) {
        case STATE_IDLE:
            printf("State: IDLE\n");
            break;
        case STATE_RUNNING:
            printf("State: RUNNING\n");
            break;
        case STATE_PAUSED:
            printf("State: PAUSED\n");
            break;
        case STATE_EXIT:
            printf("State: EXIT\n");
            break;
    }
}
