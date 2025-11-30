/*
 * reactive.h - Lightweight Reactive Programming for C
 *
 * Usage:
 *   #define REACTIVE_IMPLEMENTATION
 *   #include "reactive.h"
 *
 * Example:
 *   reactive_t clicks = reactive_create();
 *   reactive_subscribe(&clicks, my_callback, my_context);
 *   reactive_emit(&clicks, 42);
 */

#ifndef REACTIVE_H
#define REACTIVE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ==================== CONFIGURATION ==================== */
#ifndef REACTIVE_MAX_SUBSCRIBERS
#define REACTIVE_MAX_SUBSCRIBERS 16
#endif

#ifndef REACTIVE_MAX_OPERATORS
#define REACTIVE_MAX_OPERATORS 8
#endif

/* ==================== TYPES ==================== */
typedef int reactive_value_t;

typedef void (*reactive_callback_t)(void *ctx, reactive_value_t value);
typedef reactive_value_t (*reactive_map_fn_t)(reactive_value_t value);
typedef bool (*reactive_filter_fn_t)(reactive_value_t value);

typedef struct {
  reactive_callback_t callback;
  void *ctx;
  bool active;
} reactive_subscriber_t;

typedef struct reactive_stream reactive_stream_t;

struct reactive_stream {
  reactive_subscriber_t subscribers[REACTIVE_MAX_SUBSCRIBERS];
  int subscriber_count;

  /* For operators */
  reactive_stream_t *source;
  void *operator_ctx;
  void (*operator_fn)(reactive_stream_t *self, reactive_value_t value);
};

typedef reactive_stream_t reactive_t;

/* ==================== CORE API ==================== */

/* Create a new reactive stream */
static inline reactive_t reactive_create(void) {
  reactive_t stream = {0};
  return stream;
}

/* Subscribe to a stream */
bool reactive_subscribe(reactive_t *stream, reactive_callback_t callback,
                        void *ctx);

/* Emit a value to all subscribers */
void reactive_emit(reactive_t *stream, reactive_value_t value);

/* Unsubscribe a callback */
void reactive_unsubscribe(reactive_t *stream, reactive_callback_t callback,
                          void *ctx);

/* Clear all subscriptions */
void reactive_clear(reactive_t *stream);

/* ==================== OPERATORS ==================== */

/* Map: transform values */
reactive_t reactive_map(reactive_t *source, reactive_map_fn_t transform);

/* Filter: only pass values that match predicate */
reactive_t reactive_filter(reactive_t *source, reactive_filter_fn_t predicate);

/* Debounce: only emit after delay (requires manual time tracking) */
typedef struct {
  float last_time;
  float delay;
  float (*get_time)(void);
} reactive_debounce_ctx_t;

reactive_t reactive_debounce(reactive_t *source, float delay_seconds,
                             float (*get_time)(void));

/* Scan: accumulate values (like reduce but emits each step) */
typedef struct {
  reactive_value_t accumulator;
  reactive_value_t (*reducer)(reactive_value_t acc, reactive_value_t val);
} reactive_scan_ctx_t;

reactive_t reactive_scan(reactive_t *source, reactive_value_t initial,
                         reactive_value_t (*reducer)(reactive_value_t acc,
                                                     reactive_value_t val));

/* Take: only take first N emissions */
typedef struct {
  int remaining;
} reactive_take_ctx_t;

reactive_t reactive_take(reactive_t *source, int count);

/* ==================== UTILITIES ==================== */

/* Check if stream has any active subscribers */
static inline bool reactive_has_subscribers(reactive_t *stream) {
  for (int i = 0; i < stream->subscriber_count; i++) {
    if (stream->subscribers[i].active)
      return true;
  }
  return false;
}

/* Get subscriber count */
static inline int reactive_get_subscriber_count(reactive_t *stream) {
  int count = 0;
  for (int i = 0; i < stream->subscriber_count; i++) {
    if (stream->subscribers[i].active)
      count++;
  }
  return count;
}

/* ==================== IMPLEMENTATION ==================== */
#ifdef REACTIVE_IMPLEMENTATION

bool reactive_subscribe(reactive_t *stream, reactive_callback_t callback,
                        void *ctx) {
  if (!stream || !callback)
    return false;
  if (stream->subscriber_count >= REACTIVE_MAX_SUBSCRIBERS)
    return false;

  stream->subscribers[stream->subscriber_count] =
      (reactive_subscriber_t){.callback = callback, .ctx = ctx, .active = true};
  stream->subscriber_count++;
  return true;
}

void reactive_emit(reactive_t *stream, reactive_value_t value) {
  if (!stream)
    return;

  /* If this stream has an operator, apply it first */
  if (stream->operator_fn) {
    stream->operator_fn(stream, value);
    return;
  }

  /* Otherwise emit to all subscribers */
  for (int i = 0; i < stream->subscriber_count; i++) {
    if (stream->subscribers[i].active && stream->subscribers[i].callback) {
      stream->subscribers[i].callback(stream->subscribers[i].ctx, value);
    }
  }
}

void reactive_unsubscribe(reactive_t *stream, reactive_callback_t callback,
                          void *ctx) {
  if (!stream)
    return;

  for (int i = 0; i < stream->subscriber_count; i++) {
    if (stream->subscribers[i].callback == callback &&
        stream->subscribers[i].ctx == ctx) {
      stream->subscribers[i].active = false;
    }
  }
}

void reactive_clear(reactive_t *stream) {
  if (!stream)
    return;
  stream->subscriber_count = 0;
  memset(stream->subscribers, 0, sizeof(stream->subscribers));
}

/* ==================== OPERATOR IMPLEMENTATIONS ==================== */

static void map_operator(reactive_stream_t *self, reactive_value_t value) {
  reactive_map_fn_t transform = (reactive_map_fn_t)self->operator_ctx;
  reactive_value_t transformed = transform(value);

  /* Emit transformed value to this stream's subscribers */
  for (int i = 0; i < self->subscriber_count; i++) {
    if (self->subscribers[i].active && self->subscribers[i].callback) {
      self->subscribers[i].callback(self->subscribers[i].ctx, transformed);
    }
  }
}

reactive_t reactive_map(reactive_t *source, reactive_map_fn_t transform) {
  reactive_t mapped = reactive_create();
  mapped.source = source;
  mapped.operator_ctx = (void *)transform;
  mapped.operator_fn = map_operator;

  /* Subscribe to source and forward through operator */
  reactive_subscribe(source, (reactive_callback_t)reactive_emit, &mapped);
  return mapped;
}

static void filter_operator(reactive_stream_t *self, reactive_value_t value) {
  reactive_filter_fn_t predicate = (reactive_filter_fn_t)self->operator_ctx;

  if (predicate(value)) {
    for (int i = 0; i < self->subscriber_count; i++) {
      if (self->subscribers[i].active && self->subscribers[i].callback) {
        self->subscribers[i].callback(self->subscribers[i].ctx, value);
      }
    }
  }
}

reactive_t reactive_filter(reactive_t *source, reactive_filter_fn_t predicate) {
  reactive_t filtered = reactive_create();
  filtered.source = source;
  filtered.operator_ctx = (void *)predicate;
  filtered.operator_fn = filter_operator;

  reactive_subscribe(source, (reactive_callback_t)reactive_emit, &filtered);
  return filtered;
}

static void debounce_operator(reactive_stream_t *self, reactive_value_t value) {
  reactive_debounce_ctx_t *ctx = (reactive_debounce_ctx_t *)self->operator_ctx;
  float now = ctx->get_time();

  if (now - ctx->last_time >= ctx->delay) {
    ctx->last_time = now;
    for (int i = 0; i < self->subscriber_count; i++) {
      if (self->subscribers[i].active && self->subscribers[i].callback) {
        self->subscribers[i].callback(self->subscribers[i].ctx, value);
      }
    }
  }
}

reactive_t reactive_debounce(reactive_t *source, float delay_seconds,
                             float (*get_time)(void)) {
  static reactive_debounce_ctx_t debounce_ctx;
  debounce_ctx.last_time = 0;
  debounce_ctx.delay = delay_seconds;
  debounce_ctx.get_time = get_time;

  reactive_t debounced = reactive_create();
  debounced.source = source;
  debounced.operator_ctx = &debounce_ctx;
  debounced.operator_fn = debounce_operator;

  reactive_subscribe(source, (reactive_callback_t)reactive_emit, &debounced);
  return debounced;
}

static void scan_operator(reactive_stream_t *self, reactive_value_t value) {
  reactive_scan_ctx_t *ctx = (reactive_scan_ctx_t *)self->operator_ctx;
  ctx->accumulator = ctx->reducer(ctx->accumulator, value);

  for (int i = 0; i < self->subscriber_count; i++) {
    if (self->subscribers[i].active && self->subscribers[i].callback) {
      self->subscribers[i].callback(self->subscribers[i].ctx, ctx->accumulator);
    }
  }
}

reactive_t reactive_scan(reactive_t *source, reactive_value_t initial,
                         reactive_value_t (*reducer)(reactive_value_t acc,
                                                     reactive_value_t val)) {
  static reactive_scan_ctx_t scan_ctx;
  scan_ctx.accumulator = initial;
  scan_ctx.reducer = reducer;

  reactive_t scanned = reactive_create();
  scanned.source = source;
  scanned.operator_ctx = &scan_ctx;
  scanned.operator_fn = scan_operator;

  reactive_subscribe(source, (reactive_callback_t)reactive_emit, &scanned);
  return scanned;
}

static void take_operator(reactive_stream_t *self, reactive_value_t value) {
  reactive_take_ctx_t *ctx = (reactive_take_ctx_t *)self->operator_ctx;

  if (ctx->remaining > 0) {
    ctx->remaining--;
    for (int i = 0; i < self->subscriber_count; i++) {
      if (self->subscribers[i].active && self->subscribers[i].callback) {
        self->subscribers[i].callback(self->subscribers[i].ctx, value);
      }
    }
  }
}

reactive_t reactive_take(reactive_t *source, int count) {
  static reactive_take_ctx_t take_ctx;
  take_ctx.remaining = count;

  reactive_t taken = reactive_create();
  taken.source = source;
  taken.operator_ctx = &take_ctx;
  taken.operator_fn = take_operator;

  reactive_subscribe(source, (reactive_callback_t)reactive_emit, &taken);
  return taken;
}

#endif /* REACTIVE_IMPLEMENTATION */

#ifdef __cplusplus
}
#endif

#endif /* REACTIVE_H */
