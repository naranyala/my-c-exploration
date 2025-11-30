#define RLIGHTS_IMPLEMENTATION
#include "raylib.h"
#include <math.h>
#include <stdio.h>

#define AREACTIVE_NO_STDIO
#include "ano_reactivity.h"

/* -------------------------
   Type definitions
   ------------------------- */

typedef RSIGNAL(int) SignalInt;
typedef RCOMPUTED(int) ComputedInt;

/* -------------------------
   Reactive compute function
   ------------------------- */

typedef struct {
  ComputedInt *dbl;
  SignalInt *count;
} DoubleCtx;

/* When count changes, recompute double_count */
static void compute_double(REffect *self, void *ctx_void) {
  DoubleCtx *ctx = (DoubleCtx *)ctx_void;
  int base = rs_get(ctx->count);
  rc_set_value(ctx->dbl, base * 2);
}

/* -------------------------
   Draw effect
   ------------------------- */

typedef struct {
  SignalInt *count;
  ComputedInt *dbl;
} DrawCtx;

/* draw everything (executed every frame) */
static void draw_effect(REffect *e, void *ctx_void) {
  DrawCtx *ctx = (DrawCtx *)ctx_void;

  BeginDrawing();
  ClearBackground(BLACK);

  DrawText("Reactive Counter Demo", 20, 20, 32, RAYWHITE);

  char buf[128];
  sprintf(buf, "Count: %d", rs_get(ctx->count));
  DrawText(buf, 20, 120, 40, GREEN);

  sprintf(buf, "Double: %d", rc_get(ctx->dbl));
  DrawText(buf, 20, 180, 40, SKYBLUE);

  DrawText("Press UP / DOWN to change count", 20, 260, 24, GRAY);

  EndDrawing();
}

/* -------------------------
   main()
   ------------------------- */

int main(void) {
  InitWindow(600, 400, "Reactive Counter Demo");
  SetTargetFPS(60);

  /* ---- Reactive primitives ---- */

  SignalInt count;
  rs_init(&count, 0);

  ComputedInt double_count;
  /* computed context */
  DoubleCtx dctx = {.dbl = &double_count, .count = &count};
  rc_init(&double_count, compute_double, &dctx);

  /* draw effect (runs every frame) */
  REffect drawEff;
  DrawCtx drawCtx = {.count = &count, .dbl = &double_count};
  reffect_init(&drawEff, draw_effect, &drawCtx);

  /* schedule draw effect once so it runs every frame */
  reffect_schedule(&drawEff);

  /* ---- main loop ---- */
  while (!WindowShouldClose()) {

    /* update count on input */
    if (IsKeyPressed(KEY_UP)) {
      rs_set(&count, rs_get(&count) + 1);
    }
    if (IsKeyPressed(KEY_DOWN)) {
      rs_set(&count, rs_get(&count) - 1);
    }

    /* run reactive queue for this frame */
    r_flush();

    /* because drawing is an effect, re-schedule it every frame */
    reffect_schedule(&drawEff);
  }

  /* cleanup */
  rc_free(&double_count);
  rs_free(&count);
  reffect_free(&drawEff);
  r_shutdown();

  CloseWindow();
  return 0;
}
