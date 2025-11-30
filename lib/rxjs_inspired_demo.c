/* demo.c - Demonstration of reactive.h library */
#define REACTIVE_IMPLEMENTATION
#include "rxjs_inspired.h"

#include "raylib.h"
#include <stdio.h>

/* ==================== State ==================== */
static int counter = 0;
static int scan_total = 0;
static int particle_count = 0;
static Vector2 particles[32];
static float particle_life[32];
static bool show_debounce = false;
static bool show_take = false;
static int take_count = 0;

/* ==================== Callbacks ==================== */
static void on_raw_click(void *ctx, reactive_value_t value) {
  (void)ctx;
  printf("🖱️  Raw click: %d\n", value);
}

static void on_doubled(void *ctx, reactive_value_t value) {
  (void)ctx;
  counter = value;
}

static void on_even_particle(void *ctx, reactive_value_t value) {
  (void)ctx;
  (void)value;

  if (particle_count < 32) {
    particles[particle_count] = GetMousePosition();
    particle_life[particle_count] = 1.0f;
    particle_count++;
  }
}

static void on_debounced(void *ctx, reactive_value_t value) {
  (void)ctx;
  (void)value;
  show_debounce = true;
}

static void on_scan_total(void *ctx, reactive_value_t value) {
  (void)ctx;
  scan_total = value;
}

static void on_take_limited(void *ctx, reactive_value_t value) {
  (void)ctx;
  (void)value;
  show_take = true;
  take_count++;
}

/* ==================== Transform Functions ==================== */
static reactive_value_t multiply_by_2(reactive_value_t v) { return v * 2; }

static bool is_even(reactive_value_t v) { return v % 2 == 0; }

static reactive_value_t sum_reducer(reactive_value_t acc,
                                    reactive_value_t val) {
  return acc + val;
}

/* Wrapper for GetTime */
static float get_time_wrapper(void) { return (float)GetTime(); }

/* ==================== Main ==================== */
int main(void) {
  InitWindow(1000, 650, "Reactive.h Demo - Clean Architecture");
  SetTargetFPS(60);

  /* Create source stream */
  reactive_t clicks = reactive_create();

  /* Build reactive pipelines */
  reactive_t doubled = reactive_map(&clicks, multiply_by_2);
  reactive_t even_only = reactive_filter(&doubled, is_even);
  reactive_t debounced = reactive_debounce(&clicks, 0.3f, get_time_wrapper);
  reactive_t scan = reactive_scan(&clicks, 0, sum_reducer);
  reactive_t limited = reactive_take(&clicks, 5);

  /* Subscribe observers */
  reactive_subscribe(&clicks, on_raw_click, NULL);
  reactive_subscribe(&doubled, on_doubled, NULL);
  reactive_subscribe(&even_only, on_even_particle, NULL);
  reactive_subscribe(&debounced, on_debounced, NULL);
  reactive_subscribe(&scan, on_scan_total, NULL);
  reactive_subscribe(&limited, on_take_limited, NULL);

  Rectangle btn = {380, 240, 240, 120};

  printf("=== Reactive.h Demo Started ===\n");
  printf("📊 Pipelines active:\n");
  printf("   • clicks -> log\n");
  printf("   • clicks -> map(x2) -> counter\n");
  printf("   • clicks -> map(x2) -> filter(even) -> particles\n");
  printf("   • clicks -> debounce(300ms)\n");
  printf("   • clicks -> scan(sum) -> total\n");
  printf("   • clicks -> take(5) -> limited\n\n");

  while (!WindowShouldClose()) {
    /* Handle click events */
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
      if (CheckCollisionPointRec(GetMousePosition(), btn)) {
        reactive_emit(&clicks, 1);
        show_debounce = false;
      }
    }

    /* Update particles */
    for (int i = 0; i < particle_count; i++) {
      particle_life[i] -= 0.02f;
      particles[i].y -= 2;

      if (particle_life[i] <= 0) {
        particles[i] = particles[--particle_count];
        particle_life[i] = particle_life[particle_count];
        i--;
      }
    }

    /* Render */
    BeginDrawing();
    ClearBackground((Color){15, 15, 30, 255});

    /* Title */
    DrawText("Reactive.h Demo", 20, 20, 32, RAYWHITE);
    DrawText("Single-Header Reactive Library for C", 20, 55, 18, GRAY);

    /* Button */
    Color btn_color =
        CheckCollisionPointRec(GetMousePosition(), btn) ? MAROON : DARKBLUE;
    DrawRectangleRec(btn, btn_color);
    DrawRectangleLinesEx(btn, 4, GOLD);
    DrawText("CLICK ME", 420, 280, 36, WHITE);

    /* Stats */
    DrawRectangle(20, 100, 350, 120, (Color){20, 20, 40, 200});
    DrawRectangleLinesEx((Rectangle){20, 100, 350, 120}, 2, SKYBLUE);
    DrawText(TextFormat("Counter (x2): %d", counter), 40, 120, 28, YELLOW);
    DrawText(TextFormat("Total (scan): %d", scan_total), 40, 155, 28, LIME);
    DrawText(TextFormat("Take(5): %d/5 %s", take_count,
                        take_count >= 5 ? "DONE" : ""),
             40, 190, 28, ORANGE);

    /* Particles */
    for (int i = 0; i < particle_count; i++) {
      Color c = RED;
      c.a = (unsigned char)(particle_life[i] * 255);
      DrawCircleV(particles[i], 24, c);
    }

    /* Debounce indicator */
    if (show_debounce) {
      DrawRectangle(630, 100, 350, 120, (Color){40, 20, 20, 200});
      DrawRectangleLinesEx((Rectangle){630, 100, 350, 120}, 2, RED);
      DrawText("DEBOUNCED!", 660, 130, 32, RED);
      DrawText("(300ms delay)", 660, 170, 20, PINK);
    }

    /* Pipeline diagram */
    DrawText("Active Pipelines:", 20, 400, 24, LIME);
    DrawRectangle(20, 430, 960, 190, (Color){20, 30, 20, 150});

    DrawText("1. clicks -> log                            [Console]", 40, 445,
             16, GRAY);
    DrawText("2. clicks -> map(x2) -> counter             [Yellow Box]", 40,
             470, 16, YELLOW);
    DrawText(
        "3. clicks -> map(x2) -> filter(even) -> particles   [Red Circles]", 40,
        495, 16, RED);
    DrawText("4. clicks -> debounce(300ms)                [Red Box]", 40, 520,
             16, PINK);
    DrawText("5. clicks -> scan(sum) -> total             [Green Box]", 40, 545,
             16, LIME);
    DrawText("6. clicks -> take(5) -> limited             [Orange Box]", 40,
             570, 16, ORANGE);

    DrawText(TextFormat("Active Subscribers: %d",
                        reactive_get_subscriber_count(&clicks) +
                            reactive_get_subscriber_count(&doubled) +
                            reactive_get_subscriber_count(&even_only) +
                            reactive_get_subscriber_count(&debounced) +
                            reactive_get_subscriber_count(&scan) +
                            reactive_get_subscriber_count(&limited)),
             40, 595, 16, SKYBLUE);

    EndDrawing();
  }

  /* Cleanup */
  printf("\n=== Demo Closed ===\n");
  printf("Final stats:\n");
  printf("  Counter: %d\n", counter);
  printf("  Total: %d\n", scan_total);
  printf("  Take count: %d/5\n", take_count);

  reactive_clear(&clicks);
  reactive_clear(&doubled);
  reactive_clear(&even_only);
  reactive_clear(&debounced);
  reactive_clear(&scan);
  reactive_clear(&limited);

  CloseWindow();
  return 0;
}
