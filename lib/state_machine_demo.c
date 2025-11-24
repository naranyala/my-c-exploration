/********************************************************************
 *  traffic.c  –  ultra-simple raylib + state_machine.h demo
 *******************************************************************/
#define STATE_MACHINE_IMPLEMENTATION
#include "state_machine.h"
#include <raylib.h>

/* ----------------------------------------------------------
 *  User-defined states
 * ---------------------------------------------------------- */
typedef enum {
    TRAFFIC_RED,
    TRAFFIC_GREEN,
    TRAFFIC_YELLOW,
    TRAFFIC_NUM_STATES
} TrafficState_t;

/* Use one library event for NEXT */
#define TRAFFIC_NEXT EVENT_START

/* ----------------------------------------------------------
 *  Forward declarations
 * ---------------------------------------------------------- */
static StateResult_t red_entry   (StateMachine_t *sm, TransitionData_t d);
static StateResult_t green_entry (StateMachine_t *sm, TransitionData_t d);
static StateResult_t yellow_entry(StateMachine_t *sm, TransitionData_t d);
static StateResult_t do_timer    (StateMachine_t *sm, TransitionData_t d);

/* ----------------------------------------------------------
 *  Transition table
 * ---------------------------------------------------------- */
static const Transition_t trans[TRAFFIC_NUM_STATES][NUM_EVENTS] = {
    [TRAFFIC_RED]    = { [TRAFFIC_NEXT] = { TRAFFIC_GREEN,  NULL } },
    [TRAFFIC_GREEN]  = { [TRAFFIC_NEXT] = { TRAFFIC_YELLOW, NULL } },
    [TRAFFIC_YELLOW] = { [TRAFFIC_NEXT] = { TRAFFIC_RED,    NULL } }
};

/* ----------------------------------------------------------
 *  State definition table
 * ---------------------------------------------------------- */
static const State_Definition_t states[TRAFFIC_NUM_STATES] = {
    [TRAFFIC_RED]    = { .entry_action = red_entry,
                         .exit_action  = NULL,
                         .do_action    = do_timer },
    [TRAFFIC_GREEN]  = { .entry_action = green_entry,
                         .exit_action  = NULL,
                         .do_action    = do_timer },
    [TRAFFIC_YELLOW] = { .entry_action = yellow_entry,
                         .exit_action  = NULL,
                         .do_action    = do_timer }
};

/* ----------------------------------------------------------
 *  Timer context
 * ---------------------------------------------------------- */
typedef struct {
    float hold;
    float elapsed;
} Timer;

/* ----------------------------------------------------------
 *  Action implementations
 * ---------------------------------------------------------- */
static StateResult_t red_entry(StateMachine_t *sm, TransitionData_t d) {
    (void)d;
    Timer *t = (Timer*)sm->user_data;
    t->hold = 4.0f;
    t->elapsed = 0.0f;
    return SM_RESULT_OK;
}
static StateResult_t green_entry(StateMachine_t *sm, TransitionData_t d) {
    (void)d;
    Timer *t = (Timer*)sm->user_data;
    t->hold = 3.0f;
    t->elapsed = 0.0f;
    return SM_RESULT_OK;
}
static StateResult_t yellow_entry(StateMachine_t *sm, TransitionData_t d) {
    (void)d;
    Timer *t = (Timer*)sm->user_data;
    t->hold = 1.0f;
    t->elapsed = 0.0f;
    return SM_RESULT_OK;
}

static StateResult_t do_timer(StateMachine_t *sm, TransitionData_t d) {
    (void)d;
    Timer *t = (Timer*)sm->user_data;
    t->elapsed += GetFrameTime();
    if (t->elapsed >= t->hold)
        sm_process_event(sm, TRAFFIC_NEXT, NULL);

    if (IsKeyPressed(KEY_SPACE))
        sm_process_event(sm, TRAFFIC_NEXT, NULL);

    return SM_RESULT_OK;
}

/* ----------------------------------------------------------
 *  Helper: get state name as string
 * ---------------------------------------------------------- */
static const char* state_name(State_t s) {
    switch (s) {
        case TRAFFIC_RED:    return "RED";
        case TRAFFIC_GREEN:  return "GREEN";
        case TRAFFIC_YELLOW: return "YELLOW";
        default:             return "UNKNOWN";
    }
}

/* ----------------------------------------------------------
 *  main
 * ---------------------------------------------------------- */
int main(void) {
    InitWindow(240, 420, "Traffic Light");
    SetTargetFPS(60);

    Timer timer = {0};
    StateMachine_t sm;
    sm_init(&sm, TRAFFIC_RED, states, trans, &timer);

    while (!WindowShouldClose()) {
        sm_run_current_state_do_action(&sm);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        DrawRectangleLinesEx((Rectangle){70, 50, 100, 280}, 4, BLACK);
        DrawRectangle(75, 55, 90, 270, DARKGRAY);

        Color r = (sm.current_state == TRAFFIC_RED)    ? RED   : LIGHTGRAY;
        Color y = (sm.current_state == TRAFFIC_YELLOW) ? YELLOW: LIGHTGRAY;
        Color g = (sm.current_state == TRAFFIC_GREEN)  ? GREEN : LIGHTGRAY;

        DrawCircle(120, 100, 35, r);
        DrawCircle(120, 160, 35, y);
        DrawCircle(120, 220, 35, g);

        /* Reactive text showing current state */
        DrawText(TextFormat("STATE: %s", state_name(sm.current_state)),
                 40, 340, 20, BLACK);

        DrawText("SPACE = hurry", 50, 370, 18, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

