#ifndef REACTIVE_H
#define REACTIVE_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define MAX_DEPS 8
#define MAX_STR 256

// Signal types
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

  // For computed signals
  Signal *deps[MAX_DEPS];
  int dep_count;
  void (*compute)(Signal *self);
};

// Global registry
static Signal *g_all_signals[128]; // increased a bit
static int g_signal_count = 0;

static inline void register_signal(Signal *s) {
  if (g_signal_count < 128) {
    g_all_signals[g_signal_count++] = s;
  }
}

// ===== Create Signals =====
static inline Signal signal_int(int value) {
  Signal s = {.type = SIG_INT, .val.i = value};
  register_signal(&s); // auto-register everything
  return s;
}

static inline Signal signal_double(double value) {
  Signal s = {.type = SIG_DOUBLE, .val.d = value};
  register_signal(&s);
  return s;
}

static inline Signal signal_string(const char *value) {
  Signal s = {.type = SIG_STRING};
  strncpy(s.val.s, value, MAX_STR - 1);
  register_signal(&s);
  return s;
}

// ===== Get Values ===== (unchanged)
static inline int get_int(Signal *s) { return s->val.i; }
static inline double get_double(Signal *s) { return s->val.d; }
static inline const char *get_string(Signal *s) { return s->val.s; }

// ===== Propagation =====
static inline void propagate(Signal *changed);

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

// ===== Computed Signals FIXED =====
static inline Signal signal_computed(void (*compute)(Signal *self),
                                     Signal *deps[], int dep_count) {
  Signal s = {0};
  s.compute = compute;
  s.dep_count = dep_count > MAX_DEPS ? MAX_DEPS : dep_count;

  for (int i = 0; i < s.dep_count; i++)
    s.deps[i] = deps[i];

  register_signal(&s); // ← register BEFORE computing
  compute(&s);         // ← now deps[] are valid and we are registered
  s.dirty = false;     // initial compute is clean
  return s;
}

// ===== Propagation (slightly smarter + recursive) =====
static inline void propagate(Signal *changed) {
  for (int i = 0; i < g_signal_count; i++) {
    Signal *candidate = g_all_signals[i];
    if (!candidate->compute)
      continue;

    bool depends = false;
    for (int j = 0; j < candidate->dep_count; j++) {
      if (candidate->deps[j] == changed) {
        depends = true;
        break;
      }
    }
    if (depends) {
      candidate->dirty = true;
      candidate->compute(candidate); // recompute immediately (pull model)
    }
  }
}

#endif // REACTIVE_H
