// state_machine.h

#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#include <stdbool.h>
#include <stddef.h> // For NULL

// --- Configuration Types (User Must Define) ---

typedef enum {
    STATE_IDLE,
    STATE_PROCESSING,
    STATE_ERROR,
    NUM_STATES
} State_t;

typedef enum {
    EVENT_START,
    EVENT_DATA_RECEIVED,
    EVENT_FAILURE,
    EVENT_RESET,
    NUM_EVENTS
} Event_t;

// --- Return Status ---

typedef enum {
    SM_RESULT_OK,
    SM_RESULT_ERROR,
    SM_RESULT_IGNORED // Action executed but had no internal effect
} StateResult_t;

// --- Function Pointer & Context Definitions ---

typedef struct StateMachine StateMachine_t;

// Type for transition data passed between the event call and state actions
typedef void* TransitionData_t; 

// State action signature now returns a status
typedef StateResult_t (*StateAction_t)(StateMachine_t *sm, TransitionData_t data);

// --- Transition Table Structures ---

// Defines a single transition rule
typedef struct {
    State_t next_state;
    // Optional action to run *during* the transition (before entry/after exit)
    StateAction_t transition_action; 
} Transition_t;

// Structure to hold the details for a single state's actions
typedef struct {
    StateAction_t entry_action;
    StateAction_t exit_action;
    StateAction_t do_action; // Continues to be called by sm_run_current_state_do_action
} State_Definition_t;

// --- State Machine Context ---

struct StateMachine {
    State_t current_state;
    void *user_data;
    const State_Definition_t *state_definitions;
    // The core lookup table: [Current State][Event] -> Transition_t
    const Transition_t (*transition_table)[NUM_EVENTS]; 
};

// --- Core Function Prototypes ---

void sm_init(StateMachine_t *sm, State_t initial_state, 
             const State_Definition_t definitions[], 
             const Transition_t transition_table[][NUM_EVENTS], 
             void *data);

bool sm_process_event(StateMachine_t *sm, Event_t event, TransitionData_t data);

StateResult_t sm_run_current_state_do_action(StateMachine_t *sm);

// --- Implementation ---

#ifdef STATE_MACHINE_IMPLEMENTATION

void sm_init(StateMachine_t *sm, State_t initial_state, 
             const State_Definition_t definitions[], 
             const Transition_t transition_table[][NUM_EVENTS], 
             void *data) 
{
    if (!sm || initial_state >= NUM_STATES || !definitions || !transition_table) {
        return; 
    }
    
    sm->current_state = initial_state;
    sm->user_data = data;
    sm->state_definitions = definitions;
    sm->transition_table = transition_table;

    if (sm->state_definitions[initial_state].entry_action) {
        // Initial entry action is called with NULL transition data
        sm->state_definitions[initial_state].entry_action(sm, NULL); 
    }
}

bool sm_process_event(StateMachine_t *sm, Event_t event, TransitionData_t data) {
    if (!sm || event >= NUM_EVENTS || sm->current_state >= NUM_STATES) {
        return false;
    }

    State_t current = sm->current_state;
    const Transition_t *transition = &sm->transition_table[current][event];
    State_t next = transition->next_state;

    // Check for an actual transition (next_state != current_state)
    if (next != current) {
        // 1. Execute EXIT action for the current state
        if (sm->state_definitions[current].exit_action) {
            sm->state_definitions[current].exit_action(sm, data);
        }

        // 2. Execute dedicated TRANSITION action
        if (transition->transition_action) {
            transition->transition_action(sm, data);
        }

        // 3. Update the state
        sm->current_state = next;

        // 4. Execute ENTRY action for the new state
        if (sm->state_definitions[next].entry_action) {
            sm->state_definitions[next].entry_action(sm, data);
        }
        return true; 
    }
    
    return false; // No transition
}

StateResult_t sm_run_current_state_do_action(StateMachine_t *sm) {
    if (!sm) return SM_RESULT_ERROR;
    
    StateAction_t do_action = sm->state_definitions[sm->current_state].do_action;
    if (do_action) {
        // Do actions are always called with NULL transition data
        return do_action(sm, NULL); 
    }
    return SM_RESULT_OK;
}

#endif // STATE_MACHINE_IMPLEMENTATION
#endif // STATE_MACHINE_H
