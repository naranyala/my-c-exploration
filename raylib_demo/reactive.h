#ifndef REACTIVE_H
#define REACTIVE_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#define MAX_DEPS 8
#define MAX_STR 256

// Signal types
typedef struct Signal Signal;

typedef enum {
    SIG_INT,
    SIG_DOUBLE,
    SIG_STRING
} SignalType;

struct Signal {
    SignalType type;
    bool dirty;
    
    union {
        int i;
        double d;
        char s[MAX_STR];
    } val;
    
    // For computed signals
    Signal *deps[MAX_DEPS];
    int dep_count;
    void (*compute)(Signal *self);
};

// ===== Create Signals =====

static inline Signal signal_int(int value) {
    Signal s = {0};
    s.type = SIG_INT;
    s.val.i = value;
    return s;
}

static inline Signal signal_double(double value) {
    Signal s = {0};
    s.type = SIG_DOUBLE;
    s.val.d = value;
    return s;
}

static inline Signal signal_string(const char *value) {
    Signal s = {0};
    s.type = SIG_STRING;
    strncpy(s.val.s, value, MAX_STR - 1);
    return s;
}

// ===== Get Values =====

static inline int get_int(Signal *s) {
    return s->val.i;
}

static inline double get_double(Signal *s) {
    return s->val.d;
}

static inline const char* get_string(Signal *s) {
    return s->val.s;
}

// ===== Update & Auto-propagate =====

static inline void update_computed(Signal *s) {
    if (s->compute) {
        s->compute(s);
        s->dirty = false;
    }
}

static inline void propagate(Signal *s);

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

// ===== Computed Signals =====

// Global registry to track all signals
static Signal *g_all_signals[64];
static int g_signal_count = 0;

static inline void register_signal(Signal *s) {
    if (g_signal_count < 64) {
        g_all_signals[g_signal_count++] = s;
    }
}

static inline void propagate(Signal *changed) {
    // Find and update all computed signals that depend on 'changed'
    for (int i = 0; i < g_signal_count; i++) {
        Signal *s = g_all_signals[i];
        if (!s->compute) continue;
        
        // Check if this computed signal depends on 'changed'
        for (int j = 0; j < s->dep_count; j++) {
            if (s->deps[j] == changed) {
                s->dirty = true;
                update_computed(s);
                break;
            }
        }
    }
}

static inline Signal signal_computed(
    void (*compute)(Signal *self),
    Signal *deps[],
    int dep_count
) {
    Signal s = {0};
    s.compute = compute;
    s.dep_count = dep_count;
    
    for (int i = 0; i < dep_count && i < MAX_DEPS; i++) {
        s.deps[i] = deps[i];
    }
    
    // Initial compute
    compute(&s);
    return s;
}

// ===== Manual Check (if needed) =====

static inline bool is_dirty(Signal *s) {
    if (s->dirty) return true;
    
    // Check dependencies
    for (int i = 0; i < s->dep_count; i++) {
        if (s->deps[i]->dirty) return true;
    }
    return false;
}

static inline void mark_clean(Signal *s) {
    s->dirty = false;
}

#endif // REACTIVE_H
