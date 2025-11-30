
/* another_reactivity.h */

#ifndef ANO_REACTIVE_H
#define ANO_REACTIVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------
   Configuration
   --------------------- */

/* If user defines ANO_REACTIVE_NO_STDIO, library won't print debug messages */
#ifndef ANO_REACTIVE_NO_STDIO
#define RLOG(fmt, ...)                                                         \
  do {                                                                         \
    fprintf(stderr, "[reactive] " fmt "\n", ##__VA_ARGS__);                    \
  } while (0)
#else
#define RLOG(fmt, ...) (void)0
#endif

/* Simple allocator hooks */
#ifndef ANO_REACTIVE_MALLOC
#define ANO_REACTIVE_MALLOC(sz) malloc(sz)
#define ANO_REACTIVE_REALLOC(ptr, sz) realloc(ptr, sz)
#define ANO_REACTIVE_FREE(ptr) free(ptr)
#endif

/* ---------------------
   Low-level utils
   --------------------- */

static inline void *r_memdup(const void *src, size_t n) {
  if (!src || n == 0)
    return NULL;
  void *p = ANO_REACTIVE_MALLOC(n);
  if (!p)
    return NULL;
  memcpy(p, src, n);
  return p;
}

/* Small dynamic array helpers for void* arrays */
typedef struct {
  void **data;
  int count;
  int cap;
} RPtrArr;

static inline void rparr_init(RPtrArr *a) {
  a->data = NULL;
  a->count = 0;
  a->cap = 0;
}
static inline void rparr_free(RPtrArr *a) {
  if (a->data)
    ANO_REACTIVE_FREE(a->data);
  a->data = NULL;
  a->count = 0;
  a->cap = 0;
}
static inline void rparr_push(RPtrArr *a, void *p) {
  if (a->count == a->cap) {
    int newcap = a->cap ? a->cap * 2 : 4;
    void **newd = ANO_REACTIVE_REALLOC(a->data, newcap * sizeof(void *));
    if (!newd)
      return;
    a->data = newd;
    a->cap = newcap;
  }
  a->data[a->count++] = p;
}
static inline void rparr_remove(RPtrArr *a, void *p) {
  for (int i = 0; i < a->count; ++i) {
    if (a->data[i] == p) {
      memmove(&a->data[i], &a->data[i + 1],
              (a->count - i - 1) * sizeof(void *));
      a->count--;
      return;
    }
  }
}

/* ---------------------
   Core reactive structures
   --------------------- */

typedef struct REffect REffect;

/* Base for signals/computed to keep subscriber list and flags */
typedef struct {
  RPtrArr subscribers; /* REffect* subscribers */
  bool dirty;          /* set when needs recompute/run */
} RSignalBase;

/* Effect callback type */
typedef void (*REffectFn)(REffect *e, void *ctx);

/* Effect structure */
struct REffect {
  REffectFn fn;
  void *ctx;
  bool running;
  bool enqueued;
  RPtrArr deps; /* signals this effect depends on (RSignalBase*) */
};

/* Global current effect pointer used during evaluation for dependency tracking
 */
static REffect *r_current_effect = NULL;

/* Effect queue for microtask-style flush */
typedef struct {
  REffect **data;
  int count;
  int cap;
} REffectQueue;

static REffectQueue r_queue = {NULL, 0, 0};

/* Helper push into queue (deduplicate) */
static void r_queue_push(REffect *e) {
  if (e->enqueued)
    return;
  if (r_queue.count == r_queue.cap) {
    int newcap = r_queue.cap ? r_queue.cap * 2 : 8;
    REffect **n =
        ANO_REACTIVE_REALLOC(r_queue.data, newcap * sizeof(REffect *));
    if (!n)
      return;
    r_queue.data = n;
    r_queue.cap = newcap;
  }
  r_queue.data[r_queue.count++] = e;
  e->enqueued = true;
}

/* Clear queue storage */
static void r_queue_clear(void) {
  if (r_queue.data)
    ANO_REACTIVE_FREE(r_queue.data);
  r_queue.data = NULL;
  r_queue.count = 0;
  r_queue.cap = 0;
}

/* ---------------------
   Dependency tracking helpers
   --------------------- */

/* When an effect reads a signal, the signal should add the effect to its
   subscribers, and the effect should record the dependency. */
static void r_signalbase_add_subscriber(RSignalBase *s, REffect *e) {
  if (!s || !e)
    return;
  /* add e to s->subscribers */
  /* avoid duplicate */
  for (int i = 0; i < s->subscribers.count; ++i)
    if (s->subscribers.data[i] == e)
      return;
  rparr_push(&s->subscribers, e);

  /* add s to e->deps (avoid duplicate) */
  for (int i = 0; i < e->deps.count; ++i)
    if (e->deps.data[i] == s)
      return;
  rparr_push(&e->deps, s);
}

/* Remove all dependencies for an effect (called before re-run to re-track) */
static void r_effect_clear_deps(REffect *e) {
  if (!e)
    return;
  for (int i = 0; i < e->deps.count; ++i) {
    RSignalBase *s = (RSignalBase *)e->deps.data[i];
    if (!s)
      continue;
    rparr_remove(&s->subscribers, e);
  }
  rparr_free(&e->deps);
  rparr_init(&e->deps);
}

/* Mark a subscriber effect dirty and enqueue it */
static void r_signalbase_mark_subs_dirty(RSignalBase *s) {
  if (!s)
    return;
  for (int i = 0; i < s->subscribers.count; ++i) {
    REffect *e = (REffect *)s->subscribers.data[i];
    if (!e)
      continue;
    /* mark dirty by enqueuing; avoid enqueuing while running */
    r_queue_push(e);
  }
}

/* ---------------------
   Effect API
   --------------------- */

static void reffect_init(REffect *e, REffectFn fn, void *ctx) {
  if (!e)
    return;
  e->fn = fn;
  e->ctx = ctx;
  e->running = false;
  e->enqueued = false;
  rparr_init(&e->deps);
}

/* Run an effect immediately (internal) */
static void reffect_run(REffect *e) {
  if (!e || !e->fn)
    return;
  e->enqueued = false;
  e->running = true;

  /* clear previous deps */
  r_effect_clear_deps(e);

  REffect *prev = r_current_effect;
  r_current_effect = e;
  e->fn(e, e->ctx);
  r_current_effect = prev;

  e->running = false;
}

/* Enqueue effect for next flush */
static void reffect_schedule(REffect *e) {
  if (!e)
    return;
  r_queue_push(e);
}

/* Dispose effect */
static void reffect_free(REffect *e) {
  if (!e)
    return;
  r_effect_clear_deps(e);
  /* nothing else to free in this simple design */
}

/* Flush queue and run all scheduled effects (in insertion order) */
static void r_flush(void) {
  /* Run until queue drains (effects may schedule others) */
  int idx = 0;
  while (idx < r_queue.count) {
    REffect *e = r_queue.data[idx++];
    if (!e)
      continue;
    /* ensure not already running */
    if (e->running)
      continue;
    reffect_run(e);
  }
  /* reset flags on remaining (they should be false) */
  for (int i = 0; i < r_queue.count; ++i)
    if (r_queue.data[i])
      r_queue.data[i]->enqueued = false;
  r_queue.count = 0;
}

/* String signal variant (uses dynamic char* for easy assignment) */
#define RSIGNAL_STR(bufsize)                                                   \
  struct {                                                                     \
    RSignalBase base;                                                          \
    char *value;                                                               \
  }

#define rs_init_str(sigptr, initial)                                           \
  do {                                                                         \
    memset(&(sigptr)->base, 0, sizeof(RSignalBase));                           \
    rparr_init(&((sigptr)->base.subscribers));                                 \
    (sigptr)->base.dirty = false;                                              \
    size_t _len = strlen(initial) + 1;                                         \
    (sigptr)->value = r_memdup(initial, _len);                                 \
  } while (0)

#define rs_free_str(sigptr)                                                    \
  do {                                                                         \
    rparr_free(&((sigptr)->base.subscribers));                                 \
    if ((sigptr)->value)                                                       \
      ANO_REACTIVE_FREE((sigptr)->value);                                      \
  } while (0)

#define rs_set_force_str(sigptr, newval)                                       \
  do {                                                                         \
    if ((sigptr)->value)                                                       \
      ANO_REACTIVE_FREE((sigptr)->value);                                      \
    size_t _len = strlen(newval) + 1;                                          \
    (sigptr)->value = r_memdup(newval, _len);                                  \
    r_signalbase_mark_subs_dirty(&(sigptr)->base);                             \
  } while (0)

/* ---------------------
   Signals (typed via macro)
   --------------------- */

/* Usage:
     RSIGNAL(type) foo;
     rs_init(&foo, initial_value);
     rs_set(&foo, new_value);
     type v = rs_get(&foo); // tracks dependency if called inside an effect
*/

#define RSIGNAL(T)                                                             \
  struct {                                                                     \
    RSignalBase base;                                                          \
    T value;                                                                   \
  }

#define rs_init(sigptr, initial)                                               \
  do {                                                                         \
    memset(&(sigptr)->base, 0, sizeof(RSignalBase));                           \
    rparr_init(&((sigptr)->base.subscribers));                                 \
    (sigptr)->base.dirty = false;                                              \
    (sigptr)->value = (initial);                                               \
  } while (0)

#define rs_free(sigptr)                                                        \
  do {                                                                         \
    rparr_free(&((sigptr)->base.subscribers));                                 \
  } while (0)

/* get returns value and registers dependency if within effect */
#define rs_get(sigptr)                                                         \
  ((r_current_effect                                                           \
        ? (r_signalbase_add_subscriber(&(sigptr)->base, r_current_effect), 0)  \
        : 0),                                                                  \
   (sigptr)->value)

/* set value (assign & mark dependents) */
#define rs_set(sigptr, newval)                                                 \
  do {                                                                         \
    typeof((sigptr)->value) _nv = (newval);                                    \
    if (memcmp(&_nv, &((sigptr)->value), sizeof(_nv)) != 0) {                  \
      (sigptr)->value = _nv;                                                   \
      r_signalbase_mark_subs_dirty(&(sigptr)->base);                           \
    }                                                                          \
  } while (0)

/* set without equality check (force) */
#define rs_set_force(sigptr, newval)                                           \
  do {                                                                         \
    (sigptr)->value = (newval);                                                \
    r_signalbase_mark_subs_dirty(&(sigptr)->base);                             \
  } while (0)

/* ---------------------
   Computed values
   --------------------- */

/* computed creation macro:
   RCOMPUTED(type) comp;
   rc_init(&comp, compute_fn, ctx);
   where compute_fn has prototype: void compute_fn(REffect *self, void *ctx)
   and should call rs_set on comp's internal signal via rc_set_value helper or
   write directly.
*/

#define RCOMPUTED(T)                                                           \
  struct {                                                                     \
    RSignalBase base;                                                          \
    T cached;                                                                  \
    REffect _internal;                                                         \
    bool initialized;                                                          \
  }

/* Internal helper to get pointer to computed's cached value (for compute
 * function) */
#define rc_get_cached_ptr(comp_ptr) (&(comp_ptr)->cached)

/* Initialize computed: compute_fn will be run to fill value and will use rs_get
   on other signals. compute_fn signature: void compute_fn(REffect *self, void
   *ctx) When compute_fn runs, it should assign new value to comp_ptr->cached
   and mark dependents if changed.
*/
#define rc_init(comp_ptr, compute_fn, ctx)                                     \
  do {                                                                         \
    memset(&(comp_ptr)->base, 0, sizeof(RSignalBase));                         \
    rparr_init(&((comp_ptr)->base.subscribers));                               \
    (comp_ptr)->base.dirty = false;                                            \
    (comp_ptr)->initialized = false;                                           \
    reffect_init(&(comp_ptr)->_internal, (REffectFn)(compute_fn), (ctx));      \
    /* store a reference to the computed object as effect ctx if needed */     \
    /* run once to establish initial value and dependencies */                 \
    reffect_run(&(comp_ptr)->_internal);                                       \
    (comp_ptr)->initialized = true;                                            \
  } while (0)

/* Set computed's cached value and mark dependents if changed.
   Use inside compute_fn to write the computed value.
*/
#define rc_set_value(comp_ptr, newval)                                         \
  do {                                                                         \
    typeof((comp_ptr)->cached) _nv = (newval);                                 \
    if (!(comp_ptr)->initialized ||                                            \
        memcmp(&_nv, &((comp_ptr)->cached), sizeof(_nv)) != 0) {               \
      (comp_ptr)->cached = _nv;                                                \
      r_signalbase_mark_subs_dirty(&(comp_ptr)->base);                         \
    }                                                                          \
    (comp_ptr)->initialized = true;                                            \
  } while (0)

#define rc_get(comp_ptr)                                                       \
  ((r_current_effect                                                           \
        ? (r_signalbase_add_subscriber(&(comp_ptr)->base, r_current_effect),   \
           0)                                                                  \
        : 0),                                                                  \
   (comp_ptr)->cached)

/* computed free */
#define rc_free(comp_ptr)                                                      \
  do {                                                                         \
    rparr_free(&((comp_ptr)->base.subscribers));                               \
    reffect_free(&((comp_ptr)->_internal));                                    \
  } while (0)

/* ---------------------
   Convenience: batch transactions
   --------------------- */

static int r_batch_count = 0;
static REffectQueue r_batch_queue = {NULL, 0, 0};

/* When batching, we intercept scheduled effects and collect them; when batch
 * ends, we move them into main queue */
static void r_batch_queue_push(REffect *e) {
  /* dedupe like main queue */
  for (int i = 0; i < r_batch_queue.count; ++i)
    if (r_batch_queue.data[i] == e)
      return;
  if (r_batch_queue.count == r_batch_queue.cap) {
    int newcap = r_batch_queue.cap ? r_batch_queue.cap * 2 : 8;
    REffect **n =
        ANO_REACTIVE_REALLOC(r_batch_queue.data, newcap * sizeof(REffect *));
    if (!n)
      return;
    r_batch_queue.data = n;
    r_batch_queue.cap = newcap;
  }
  r_batch_queue.data[r_batch_queue.count++] = e;
}

/* Start batch (suspend flush until end) */
static void r_batch_start(void) { r_batch_count++; }

/* End batch (move batch queue to main and flush if outermost) */
static void r_batch_end(void) {
  if (r_batch_count <= 0)
    return;
  r_batch_count--;
  if (r_batch_count == 0) {
    /* move batch queued effects into main queue (dedupe possible) */
    for (int i = 0; i < r_batch_queue.count; ++i) {
      REffect *e = r_batch_queue.data[i];
      if (e)
        r_queue_push(e);
    }
    /* clear batch queue */
    if (r_batch_queue.data) {
      ANO_REACTIVE_FREE(r_batch_queue.data);
      r_batch_queue.data = NULL;
    }
    r_batch_queue.count = 0;
    r_batch_queue.cap = 0;
    /* flush main queue */
    r_flush();
  }
}

/* ---------------------
   Utilities / cleanup
   --------------------- */

static void r_shutdown(void) {
  /* free main queue storage */
  if (r_queue.data)
    ANO_REACTIVE_FREE(r_queue.data);
  r_queue.data = NULL;
  r_queue.count = 0;
  r_queue.cap = 0;

  if (r_batch_queue.data)
    ANO_REACTIVE_FREE(r_batch_queue.data);
  r_batch_queue.data = NULL;
  r_batch_queue.count = 0;
  r_batch_queue.cap = 0;

  /* reset global pointers */
  r_current_effect = NULL;
}

/* ---------------------
   Example integration with raylib (guarded)
   --------------------- */

#ifdef ANO_REACTIVE_RAYLIB_EXAMPLE
/* To compile example: define ANO_REACTIVE_RAYLIB_EXAMPLE and include raylib
 * headers before including this file */
#include "raylib.h"

#endif /* ANO_REACTIVE_RAYLIB_EXAMPLE */

#ifdef __cplusplus
}
#endif

#endif /* ANO_REACTIVE_H */
