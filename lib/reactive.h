#ifndef REACTIVE_H
#define REACTIVE_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_DEPS 8
#define MAX_STR 256
#define MAX_SIGNALS 256

typedef struct Signal Signal;
typedef enum { SIG_INT, SIG_DOUBLE, SIG_STRING } SignalType;

struct Signal {
    SignalType type;
    bool dirty;
    
    union {
        int i;
        double d;
        char s[MAX_STR];
    } val;
    
    Signal *deps[MAX_DEPS];
    int dep_count;
    void (*compute)(Signal *self);
};

// Global registry
static Signal *g_all_signals[MAX_SIGNALS];
static int g_signal_count = 0;

static inline void register_signal(Signal *s) {
    if (g_signal_count < MAX_SIGNALS) {
        g_all_signals[g_signal_count++] = s;
    }
}

// Forward declaration
static inline void propagate(Signal *changed);

// ===== Create Signals (returns pointer, heap-allocated) =====
static inline Signal* signal_int(int value) {
    Signal *s = malloc(sizeof(Signal));
    *s = (Signal){.type = SIG_INT, .val.i = value};
    register_signal(s);
    return s;
}

static inline Signal* signal_double(double value) {
    Signal *s = malloc(sizeof(Signal));
    *s = (Signal){.type = SIG_DOUBLE, .val.d = value};
    register_signal(s);
    return s;
}

static inline Signal* signal_string(const char *value) {
    Signal *s = malloc(sizeof(Signal));
    *s = (Signal){.type = SIG_STRING};
    strncpy(s->val.s, value, MAX_STR - 1);
    register_signal(s);
    return s;
}

// ===== Get Values =====
static inline int get_int(Signal *s) { return s->val.i; }
static inline double get_double(Signal *s) { return s->val.d; }
static inline const char* get_string(Signal *s) { return s->val.s; }

// ===== Set Values (with propagation) =====
static inline void set_int(Signal *s, int value) {
    if (s->val.i != value) {
        s->val.i = value;
        s->dirty = true;
        propagate(s);
    }
}

static inline void set_double(Signal *s, double value) {
    if (s->val.d != value) {
        s->val.d = value;
        s->dirty = true;
        propagate(s);
    }
}

static inline void set_string(Signal *s, const char *value) {
    if (strcmp(s->val.s, value) != 0) {
        strncpy(s->val.s, value, MAX_STR - 1);
        s->dirty = true;
        propagate(s);
    }
}

// ===== Utility =====
static inline void touch(Signal *s) {
    s->dirty = true;
    propagate(s);
}

static inline void recompute(Signal *s) {
    if (s->compute) {
        s->dirty = true;
        s->compute(s);
    }
}

// ===== Computed Signals =====
static inline Signal* signal_computed(void (*compute)(Signal *self),
                                       Signal *deps[], int dep_count) {
    Signal *s = malloc(sizeof(Signal));
    *s = (Signal){0};
    s->compute = compute;
    s->dep_count = dep_count > MAX_DEPS ? MAX_DEPS : dep_count;
    
    for (int i = 0; i < s->dep_count; i++)
        s->deps[i] = deps[i];
    
    register_signal(s);
    compute(s);
    s->dirty = false;
    return s;
}

// ===== Propagation =====
static inline void propagate(Signal *changed) {
    for (int i = 0; i < g_signal_count; i++) {
        Signal *candidate = g_all_signals[i];
        if (!candidate->compute) continue;
        
        for (int j = 0; j < candidate->dep_count; j++) {
            if (candidate->deps[j] == changed) {
                candidate->dirty = true;
                candidate->compute(candidate);
                break;
            }
        }
    }
}

// ===== Cleanup =====
static inline void signals_reset(void) {
    for (int i = 0; i < g_signal_count; i++) {
        free(g_all_signals[i]);
        g_all_signals[i] = NULL;
    }
    g_signal_count = 0;
}

#endif // REACTIVE_H
