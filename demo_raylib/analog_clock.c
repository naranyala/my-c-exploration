/*
analog_clock_raylib.c — Raylib analog clock using the original reactive.h

This adapts the ASCII demo to a graphical window using raylib. It uses the
original reactive.h header (unsafe factory functions in that header are NOT
used — we construct static Signal objects and register them with
register_signal(&s) to avoid dangling-pointer issues).

Build (Linux example):
    gcc -std=c11 analog_clock_raylib.c -o analog_clock -lraylib -lm -lpthread
-ldl -lrt -lX11

On macOS or other platforms, link flags for raylib differ.

Run:
    ./analog_clock

Press ESC or close the window to quit.
*/

#include "../lib/reactive.h"
#include <math.h>
#include <raylib.h>
#include <stdio.h>
#include <time.h>

/* Forward declarations of compute callbacks */
void compute_second_angle(Signal *self);
void compute_minute_angle(Signal *self);
void compute_hour_angle(Signal *self);

/* Normalize degrees to [0,360) */
static double norm_deg(double d) {
  while (d < 0)
    d += 360.0;
  while (d >= 360.0)
    d -= 360.0;
  return d;
}

/* Compute callbacks: they read their deps and set_double(self, angle) */
void compute_second_angle(Signal *self) {
  int sec = get_int(self->deps[0]);
  double angle = norm_deg(sec * 6.0); // 360/60 = 6 deg per second
  set_double(self, angle);
}

void compute_minute_angle(Signal *self) {
  int minute = get_int(self->deps[0]);
  int second = get_int(self->deps[1]);
  double angle = norm_deg((minute + second / 60.0) * 6.0);
  set_double(self, angle);
}

void compute_hour_angle(Signal *self) {
  int hour = get_int(self->deps[0]);
  int minute = get_int(self->deps[1]);
  double angle = norm_deg(((hour % 12) + minute / 60.0) * 30.0);
  set_double(self, angle);
}

int main(void) {
  const int winW = 480;
  const int winH = 480;
  const int cx = winW / 2;
  const int cy = winH / 2;
  const int radius = 200;

  InitWindow(winW, winH, "Reactive analog clock (raylib)");
  SetTargetFPS(60);

  /* Base signals (int) */
  static Signal s_seconds = {0};
  static Signal s_minutes = {0};
  static Signal s_hours = {0};

  s_seconds.type = SIG_INT;
  s_seconds.val.i = 0;
  s_seconds.dep_count = 0;
  s_seconds.compute = NULL;
  register_signal(&s_seconds);

  s_minutes.type = SIG_INT;
  s_minutes.val.i = 0;
  s_minutes.dep_count = 0;
  s_minutes.compute = NULL;
  register_signal(&s_minutes);

  s_hours.type = SIG_INT;
  s_hours.val.i = 0;
  s_hours.dep_count = 0;
  s_hours.compute = NULL;
  register_signal(&s_hours);

  /* Computed signals (double angles) */
  static Signal s_second_angle = {0};
  static Signal s_minute_angle = {0};
  static Signal s_hour_angle = {0};

  s_second_angle.type = SIG_DOUBLE;
  s_second_angle.val.d = 0.0;
  s_second_angle.compute = compute_second_angle;
  s_second_angle.dep_count = 1;
  s_second_angle.deps[0] = &s_seconds;
  register_signal(&s_second_angle);

  s_minute_angle.type = SIG_DOUBLE;
  s_minute_angle.val.d = 0.0;
  s_minute_angle.compute = compute_minute_angle;
  s_minute_angle.dep_count = 2;
  s_minute_angle.deps[0] = &s_minutes;
  s_minute_angle.deps[1] = &s_seconds;
  register_signal(&s_minute_angle);

  s_hour_angle.type = SIG_DOUBLE;
  s_hour_angle.val.d = 0.0;
  s_hour_angle.compute = compute_hour_angle;
  s_hour_angle.dep_count = 2;
  s_hour_angle.deps[0] = &s_hours;
  s_hour_angle.deps[1] = &s_minutes;
  register_signal(&s_hour_angle);

  /* track previous second to avoid redundant set_int calls */
  int prev_sec = -1;

  while (!WindowShouldClose()) {
    /* update time once per frame, but only set signals if they changed */
    time_t now = time(NULL);
    struct tm tm_now;
    localtime_r(&now, &tm_now);

    if (tm_now.tm_sec != prev_sec) {
      prev_sec = tm_now.tm_sec;
      set_int(&s_hours, tm_now.tm_hour);
      set_int(&s_minutes, tm_now.tm_min);
      set_int(&s_seconds, tm_now.tm_sec);
    }

    /* read computed angles (degrees, 0==12:00, increasing clockwise) */
    double sec_ang = get_double(&s_second_angle);
    double min_ang = get_double(&s_minute_angle);
    double hour_ang = get_double(&s_hour_angle);

    /* convert to radians for drawing; use same convention as ASCII demo:
       x offset = sin(rad) * len, y offset = -cos(rad) * len so 0 deg points up
     */
    double rad_s = sec_ang * (M_PI / 180.0);
    double rad_m = min_ang * (M_PI / 180.0);
    double rad_h = hour_ang * (M_PI / 180.0);

    float sx = cx + (float)(sin(rad_s) * (radius * 0.9));
    float sy = cy - (float)(cos(rad_s) * (radius * 0.9));
    float mx = cx + (float)(sin(rad_m) * (radius * 0.75));
    float my = cy - (float)(cos(rad_m) * (radius * 0.75));
    float hx = cx + (float)(sin(rad_h) * (radius * 0.5));
    float hy = cy - (float)(cos(rad_h) * (radius * 0.5));

    BeginDrawing();
    ClearBackground(RAYWHITE);

    /* clock face */
    DrawCircle(cx, cy, radius, LIGHTGRAY);
    DrawCircleLines(cx, cy, radius, DARKGRAY);

    /* hour marks */
    for (int i = 0; i < 12; ++i) {
      double a = (i * 30.0) * (M_PI / 180.0);
      int x1 = cx + (int)round(sin(a) * (radius - 6));
      int y1 = cy - (int)round(cos(a) * (radius - 6));
      int x2 = cx + (int)round(sin(a) * (radius - 20));
      int y2 = cy - (int)round(cos(a) * (radius - 20));
      DrawLine(x1, y1, x2, y2, BLACK);
    }

    /* hands: draw hour (thick), minute, second (thin, red) */
    DrawLineEx((Vector2){cx, cy}, (Vector2){hx, hy}, 6.0f, BLACK);    // hour
    DrawLineEx((Vector2){cx, cy}, (Vector2){mx, my}, 4.0f, DARKBLUE); // minute
    DrawLineEx((Vector2){cx, cy}, (Vector2){sx, sy}, 2.0f, RED);      // second

    /* center */
    DrawCircle(cx, cy, 6, BLACK);
    DrawCircle(cx, cy, 4, WHITE);

    /* time text */
    char buf[64];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d", tm_now.tm_hour, tm_now.tm_min,
             tm_now.tm_sec);
    int tw = MeasureText(buf, 20);
    DrawText(buf, cx - tw / 2, cy + radius + 10, 20, BLACK);

    EndDrawing();
  }

  CloseWindow();
  return 0;
}
