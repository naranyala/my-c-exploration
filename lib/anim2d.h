// anim2d.h - Single-header 2D animation micro-lib (C99)
// Public domain / CC0. No warranty.
// Usage:
//   #define ANIM2D_IMPLEMENTATION
//   #include "anim2d.h"

#ifndef ANIM2D_H
#define ANIM2D_H

// -------- Config --------
#ifndef ANIM2D_STATIC
#define ANIM2D_DEF extern
#else
#define ANIM2D_DEF static
#endif

#ifdef __cplusplus
extern "C" {
#endif

// -------- Basic types --------
typedef struct {
  float x, y;
} anim_vec2;

typedef struct {
  float r, g, b, a;
} anim_rgba;

// Easing function: input t in [0,1] -> output in [0,1]
typedef float (*anim_ease_fn)(float t);

// Value types
typedef enum { ANIM_VAL_SCALAR, ANIM_VAL_VEC2, ANIM_VAL_COLOR } anim_val_kind;

// A keyframe stores start->end over [t0, t1] using easing
typedef struct {
  float t0, t1;       // local time range
  anim_ease_fn ease;  // easing
  anim_val_kind kind; // value type
  union {
    struct {
      float a;
    } scalar;
    struct {
      anim_vec2 va;
    } vec2;
    struct {
      anim_rgba ca;
    } color;
  } start;
  union {
    struct {
      float b;
    } scalar;
    struct {
      anim_vec2 vb;
    } vec2;
    struct {
      anim_rgba cb;
    } color;
  } end;
} anim_key;

// A track holds sequential keyframes on a single channel
#define ANIM_MAX_KEYS 32
typedef struct {
  int count;
  anim_key keys[ANIM_MAX_KEYS];
} anim_track;

// A clip bundles multiple tracks with playback settings
#define ANIM_MAX_TRACKS 16
typedef enum {
  ANIM_PLAY_ONCE,
  ANIM_PLAY_LOOP,
  ANIM_PLAY_PINGPONG
} anim_play_mode;

typedef struct {
  float duration; // seconds
  float time;     // local time
  float speed;    // time scale
  float delay;    // start delay
  anim_play_mode mode;
  int reverse; // internal flag for ping-pong
  int track_count;
  anim_track tracks[ANIM_MAX_TRACKS];
} anim_clip;

// Evaluated values per track at a given time
typedef struct {
  anim_val_kind kind;
  union {
    float s;
    anim_vec2 v;
    anim_rgba c;
  } value;
  int active;
} anim_eval;

// -------- Helpers --------
ANIM2D_DEF float anim_clamp01(float t);
ANIM2D_DEF float anim_lerp(float a, float b, float t);
ANIM2D_DEF anim_vec2 anim_lerp2(anim_vec2 a, anim_vec2 b, float t);
ANIM2D_DEF anim_rgba anim_lerpc(anim_rgba a, anim_rgba b, float t);

// Built-in easings
ANIM2D_DEF float anim_ease_linear(float t);
ANIM2D_DEF float anim_ease_smooth(float t); // smoothstep
ANIM2D_DEF float anim_ease_inquad(float t);
ANIM2D_DEF float anim_ease_outquad(float t);
ANIM2D_DEF float anim_ease_inoutquad(float t);

// -------- Track / clip API --------
ANIM2D_DEF void anim_track_reset(anim_track *trk);
ANIM2D_DEF int anim_track_add_scalar(anim_track *trk, float t0, float t1,
                                     float a, float b, anim_ease_fn f);
ANIM2D_DEF int anim_track_add_vec2(anim_track *trk, float t0, float t1,
                                   anim_vec2 a, anim_vec2 b, anim_ease_fn f);
ANIM2D_DEF int anim_track_add_color(anim_track *trk, float t0, float t1,
                                    anim_rgba a, anim_rgba b, anim_ease_fn f);

ANIM2D_DEF void anim_clip_reset(anim_clip *clip);
ANIM2D_DEF void anim_clip_config(anim_clip *clip, float duration, float speed,
                                 float delay, anim_play_mode mode);
ANIM2D_DEF void anim_clip_add(anim_clip *clip, anim_track trk);

// Advance time; returns whether clip is still playing
ANIM2D_DEF int anim_clip_update(anim_clip *clip, float dt);

// Evaluate all tracks at clip->time into out[] (size = clip->track_count)
ANIM2D_DEF void anim_clip_eval(const anim_clip *clip, anim_eval *out);

// -------- Math animation hooks --------
// Parametric paths return vec2 for t in [0,1]
typedef anim_vec2 (*anim_path_fn)(float t, void *user);

typedef struct {
  anim_vec2 center;
  float radius;
} anim_circle_params;

typedef struct {
  float A, B, a, b, phi;
} anim_lissajous_params;

ANIM2D_DEF anim_vec2 anim_path_circle(float t, void *user);
ANIM2D_DEF anim_vec2 anim_path_lissajous(float t, void *user);

// Sample path into a vec2 track across [0, duration]
ANIM2D_DEF void anim_track_from_path(anim_track *trk, anim_path_fn fn,
                                     void *user, float duration, int samples,
                                     anim_ease_fn ease);

// -------- Rendering callback pattern --------
// You own rendering. Example draw callback receiving evaluated values per
// track.
typedef void (*anim_draw_fn)(const anim_eval *evals, int count, void *user);

// A tiny runner combines update+eval+draw
typedef struct {
  anim_clip *clip;
  anim_draw_fn draw;
  void *user;
} anim_runner;

ANIM2D_DEF void anim_runner_step(anim_runner *r, float dt);

#ifdef __cplusplus
} // extern "C"
#endif

#endif // ANIM2D_H

// =================== Implementation ===================
#ifdef ANIM2D_IMPLEMENTATION
#include <math.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

ANIM2D_DEF float anim_clamp01(float t) { return t < 0 ? 0 : (t > 1 ? 1 : t); }
ANIM2D_DEF float anim_lerp(float a, float b, float t) {
  return a + (b - a) * t;
}
ANIM2D_DEF anim_vec2 anim_lerp2(anim_vec2 a, anim_vec2 b, float t) {
  anim_vec2 r = {anim_lerp(a.x, b.x, t), anim_lerp(a.y, b.y, t)};
  return r;
}
ANIM2D_DEF anim_rgba anim_lerpc(anim_rgba a, anim_rgba b, float t) {
  anim_rgba r = {anim_lerp(a.r, b.r, t), anim_lerp(a.g, b.g, t),
                 anim_lerp(a.b, b.b, t), anim_lerp(a.a, b.a, t)};
  return r;
}

// Easing
ANIM2D_DEF float anim_ease_linear(float t) { return anim_clamp01(t); }
ANIM2D_DEF float anim_ease_smooth(float t) {
  t = anim_clamp01(t);
  return t * t * (3.f - 2.f * t);
}
ANIM2D_DEF float anim_ease_inquad(float t) {
  t = anim_clamp01(t);
  return t * t;
}
ANIM2D_DEF float anim_ease_outquad(float t) {
  t = anim_clamp01(t);
  return 1.f - (1.f - t) * (1.f - t);
}
ANIM2D_DEF float anim_ease_inoutquad(float t) {
  t = anim_clamp01(t);
  return t < 0.5f ? 2.f * t * t : 1.f - powf(-2.f * t + 2.f, 2.f) / 2.f;
}

ANIM2D_DEF void anim_track_reset(anim_track *trk) { trk->count = 0; }
ANIM2D_DEF int anim_track_add_scalar(anim_track *trk, float t0, float t1,
                                     float a, float b, anim_ease_fn f) {
  if (trk->count >= ANIM_MAX_KEYS)
    return 0;
  anim_key k;
  k.t0 = t0;
  k.t1 = t1;
  k.ease = f ? f : anim_ease_linear;
  k.kind = ANIM_VAL_SCALAR;
  k.start.scalar.a = a;
  k.end.scalar.b = b;
  trk->keys[trk->count++] = k;
  return 1;
}
ANIM2D_DEF int anim_track_add_vec2(anim_track *trk, float t0, float t1,
                                   anim_vec2 a, anim_vec2 b, anim_ease_fn f) {
  if (trk->count >= ANIM_MAX_KEYS)
    return 0;
  anim_key k;
  k.t0 = t0;
  k.t1 = t1;
  k.ease = f ? f : anim_ease_linear;
  k.kind = ANIM_VAL_VEC2;
  k.start.vec2.va = a;
  k.end.vec2.vb = b;
  trk->keys[trk->count++] = k;
  return 1;
}
ANIM2D_DEF int anim_track_add_color(anim_track *trk, float t0, float t1,
                                    anim_rgba a, anim_rgba b, anim_ease_fn f) {
  if (trk->count >= ANIM_MAX_KEYS)
    return 0;
  anim_key k;
  k.t0 = t0;
  k.t1 = t1;
  k.ease = f ? f : anim_ease_linear;
  k.kind = ANIM_VAL_COLOR;
  k.start.color.ca = a;
  k.end.color.cb = b;
  trk->keys[trk->count++] = k;
  return 1;
}

ANIM2D_DEF void anim_clip_reset(anim_clip *clip) {
  clip->duration = 0;
  clip->time = 0;
  clip->speed = 1;
  clip->delay = 0;
  clip->mode = ANIM_PLAY_ONCE;
  clip->reverse = 0;
  clip->track_count = 0;
  memset(clip->tracks, 0, sizeof(clip->tracks));
}
ANIM2D_DEF void anim_clip_config(anim_clip *clip, float duration, float speed,
                                 float delay, anim_play_mode mode) {
  clip->duration = duration;
  clip->speed = speed;
  clip->delay = delay;
  clip->mode = mode;
}
ANIM2D_DEF void anim_clip_add(anim_clip *clip, anim_track trk) {
  if (clip->track_count < ANIM_MAX_TRACKS)
    clip->tracks[clip->track_count++] = trk;
}

static float anim_clip_wrap_time(const anim_clip *c, float t) {
  if (t < 0)
    return 0;
  if (c->mode == ANIM_PLAY_ONCE) {
    return t > c->duration ? c->duration : t;
  } else if (c->mode == ANIM_PLAY_LOOP) {
    if (c->duration <= 0)
      return 0;
    float m = fmodf(t, c->duration);
    return m < 0 ? m + c->duration : m;
  } else { // ping-pong
    if (c->duration <= 0)
      return 0;
    float cycle = c->duration * 2.f;
    float m = fmodf(t, cycle);
    if (m < 0)
      m += cycle;
    return m <= c->duration ? m : (cycle - m);
  }
}

ANIM2D_DEF int anim_clip_update(anim_clip *clip, float dt) {
  if (clip->delay > 0) {
    clip->delay -= dt;
    if (clip->delay > 0)
      return 1;
    // pass-through remaining fraction
    float overshoot = -clip->delay;
    if (overshoot > 0)
      dt = overshoot;
    else
      dt = 0;
  }
  clip->time = anim_clip_wrap_time(clip, clip->time + dt * clip->speed);
  if (clip->mode == ANIM_PLAY_ONCE)
    return clip->time < clip->duration;
  return 1;
}

static void anim_eval_key(const anim_key *k, float t, anim_eval *out) {
  float denom = k->t1 - k->t0;
  float u = (denom > 0) ? ((t - k->t0) / denom) : 0;
  u = anim_clamp01(u);
  u = k->ease ? k->ease(u) : u;
  out->kind = k->kind;
  out->active = 1;
  if (k->kind == ANIM_VAL_SCALAR)
    out->value.s = anim_lerp(k->start.scalar.a, k->end.scalar.b, u);
  else if (k->kind == ANIM_VAL_VEC2)
    out->value.v = anim_lerp2(k->start.vec2.va, k->end.vec2.vb, u);
  else
    out->value.c = anim_lerpc(k->start.color.ca, k->end.color.cb, u);
}

ANIM2D_DEF void anim_clip_eval(const anim_clip *clip, anim_eval *out) {
  for (int i = 0; i < clip->track_count; i++) {
    out[i].active = 0;
    const anim_track *trk = &clip->tracks[i];
    for (int k = 0; k < trk->count; k++) {
      const anim_key *key = &trk->keys[k];
      if (clip->time >= key->t0 && clip->time <= key->t1) {
        anim_eval_key(key, clip->time, &out[i]);
        break;
      }
      // If time beyond last key, hold last value
      if (clip->time > key->t1 && k == trk->count - 1) {
        anim_eval_key(key, key->t1, &out[i]);
      }
      // If before first key, hold first value
      if (clip->time < key->t0 && k == 0) {
        anim_eval_key(key, key->t0, &out[i]);
      }
    }
  }
}

// Paths
ANIM2D_DEF anim_vec2 anim_path_circle(float t, void *user) {
  anim_circle_params *p = (anim_circle_params *)user;
  float a = 2.f * (float)M_PI * t;
  anim_vec2 v = {p->center.x + p->radius * cosf(a),
                 p->center.y + p->radius * sinf(a)};
  return v;
}
ANIM2D_DEF anim_vec2 anim_path_lissajous(float t, void *user) {
  anim_lissajous_params *p = (anim_lissajous_params *)user;
  anim_vec2 v = {p->A * sinf(p->a * t + p->phi), p->B * sinf(p->b * t)};
  return v;
}
ANIM2D_DEF void anim_track_from_path(anim_track *trk, anim_path_fn fn,
                                     void *user, float duration, int samples,
                                     anim_ease_fn ease) {
  anim_track_reset(trk);
  if (samples < 2)
    samples = 2;
  for (int i = 0; i < samples - 1; i++) {
    float t0 = (float)i / (float)(samples - 1);
    float t1 = (float)(i + 1) / (float)(samples - 1);
    anim_vec2 a = fn(t0, user);
    anim_vec2 b = fn(t1, user);
    anim_track_add_vec2(trk, t0 * duration, t1 * duration, a, b, ease);
  }
}

// Runner
ANIM2D_DEF void anim_runner_step(anim_runner *r, float dt) {
  if (!r || !r->clip || !r->draw)
    return;
  anim_clip_update(r->clip, dt);
  anim_eval evals[ANIM_MAX_TRACKS];
  anim_clip_eval(r->clip, evals);
  r->draw(evals, r->clip->track_count, r->user);
}

#endif // ANIM2D_IMPLEMENTATION
