// snake_demo.c - SPEED CONTROL UPDATE!
#define ANO_REACTIVE_RAYLIB_EXAMPLE
#include "ano_reactivity.h"
#include "raylib.h"

#include <math.h> // for fmaxf, fminf
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ------------------------------------------------------------
// Config
// ------------------------------------------------------------
#define GRID_SIZE 20
#define CELL_SIZE 30
#define SCREEN_W (GRID_SIZE * CELL_SIZE)
#define SCREEN_H (SCREEN_W + 100)

// ------------------------------------------------------------
// Reactive state (NEW: speed!)
// ------------------------------------------------------------
RSIGNAL(int) score;
RSIGNAL(bool) game_over;
RSIGNAL(bool) paused;
RSIGNAL(float) speed; // NEW: cells per second - fully reactive!

typedef struct {
  int x, y;
} Pos;
RSIGNAL(Pos) snake_head;
RSIGNAL(Pos) food;

typedef enum { UP, DOWN, LEFT, RIGHT } Dir;
RSIGNAL(Dir) direction;
RSIGNAL(Dir) next_direction;

// Snake body
typedef struct {
  Pos *data;
  int count;
  int cap;
} SnakeBody;

static SnakeBody snake = {0};

// ------------------------------------------------------------
// Computed displays
// ------------------------------------------------------------
RCOMPUTED(char *) score_text;
RCOMPUTED(char *) speed_text; // NEW!
RCOMPUTED(char *) status_text;

// String setter helper
#define rc_set_value_str(comp_ptr, text)                                       \
  do {                                                                         \
    const char *_t = (text);                                                   \
    if (!(comp_ptr)->initialized || strcmp(_t, (comp_ptr)->cached) != 0) {     \
      if ((comp_ptr)->cached)                                                  \
        ANO_REACTIVE_FREE((comp_ptr)->cached);                                 \
      size_t len = strlen(_t) + 1;                                             \
      (comp_ptr)->cached = r_memdup(_t, len);                                  \
      r_signalbase_mark_subs_dirty(&(comp_ptr)->base);                         \
    }                                                                          \
    (comp_ptr)->initialized = true;                                            \
  } while (0)

static void compute_score_text(REffect *self, void *ctx) {
  char buf[64];
  snprintf(buf, sizeof(buf), "Score: %d", rs_get(&score));
  rc_set_value_str(&score_text, buf);
}

static void compute_speed_text(REffect *self, void *ctx) { // NEW!
  char buf[64];
  snprintf(buf, sizeof(buf), "Speed: %.1f", rs_get(&speed));
  rc_set_value_str(&speed_text, buf);
}

static void compute_status_text(REffect *self, void *ctx) {
  if (rs_get(&game_over))
    rc_set_value_str(&status_text, "GAME OVER – Press R to Restart");
  else if (rs_get(&paused))
    rc_set_value_str(&status_text, "PAUSED – Press P to Resume");
  else
    rc_set_value_str(&status_text, "←→↑↓ Move • , / . Speed • Eat red!");
}

// ------------------------------------------------------------
// Game logic (UPDATED: timer-based movement)
// ------------------------------------------------------------
static void spawn_food(void) {
  int x, y;
  bool bad;
  do {
    x = GetRandomValue(0, GRID_SIZE - 1);
    y = GetRandomValue(0, GRID_SIZE - 1);
    bad = false;
    for (int i = 0; i < snake.count; ++i) {
      if (snake.data[i].x == x && snake.data[i].y == y) {
        bad = true;
        break;
      }
    }
  } while (bad);
  rs_set_force(&food, ((Pos){x, y}));
}

static void reset_game(void) {
  if (snake.data)
    ANO_REACTIVE_FREE(snake.data);
  snake.data = NULL;
  snake.count = snake.cap = 0;

  snake.cap = 32;
  snake.data = ANO_REACTIVE_MALLOC(snake.cap * sizeof(Pos));
  snake.count = 3;
  snake.data[0] = (Pos){10, 10};
  snake.data[1] = (Pos){9, 10};
  snake.data[2] = (Pos){8, 10};

  rs_set_force(&snake_head, snake.data[0]);
  rs_set_force(&direction, RIGHT);
  rs_set_force(&next_direction, RIGHT);
  rs_set(&score, 0);
  rs_set_force(&speed, 4.0f); // Reset speed too!
  rs_set_force(&game_over, false);
  rs_set_force(&paused, false);

  spawn_food();
}

static void move_snake(void) {
  if (rs_get(&game_over) || rs_get(&paused))
    return;

  rs_set_force(&direction, rs_get(&next_direction));

  Pos head = rs_get(&snake_head);
  Pos new_head = head;

  switch (rs_get(&direction)) {
  case UP:
    new_head.y--;
    break;
  case DOWN:
    new_head.y++;
    break;
  case LEFT:
    new_head.x--;
    break;
  case RIGHT:
    new_head.x++;
    break;
  }

  if (new_head.x < 0 || new_head.x >= GRID_SIZE || new_head.y < 0 ||
      new_head.y >= GRID_SIZE) {
    rs_set_force(&game_over, true);
    return;
  }

  for (int i = 0; i < snake.count; ++i) {
    if (snake.data[i].x == new_head.x && snake.data[i].y == new_head.y) {
      rs_set_force(&game_over, true);
      return;
    }
  }

  for (int i = snake.count - 1; i > 0; --i)
    snake.data[i] = snake.data[i - 1];
  snake.data[0] = new_head;
  rs_set_force(&snake_head, new_head);

  if (new_head.x == rs_get(&food).x && new_head.y == rs_get(&food).y) {
    // Grow
    if (snake.count == snake.cap) {
      snake.cap *= 2;
      snake.data = ANO_REACTIVE_REALLOC(snake.data, snake.cap * sizeof(Pos));
    }
    snake.data[snake.count] = snake.data[snake.count - 1];
    snake.count++;

    rs_set(&score, rs_get(&score) + 10);

    // AUTO SPEED-UP on eat! 🚀
    rs_set_force(&speed, fminf(20.0f, rs_get(&speed) + 0.3f));

    spawn_food();
  }
}

// ------------------------------------------------------------
// Main (UPDATED: delta-time movement)
// ------------------------------------------------------------
int main(void) {
  InitWindow(SCREEN_W, SCREEN_H, "Reactive Snake • SPEED CONTROL!");
  SetTargetFPS(60); // Smooth 60 FPS, speed via timer
  srand(time(NULL));

  // Init signals
  rs_init(&score, 0);
  rs_init(&game_over, false);
  rs_init(&paused, false);
  rs_init(&speed, 4.0f); // NEW!
  rs_init(&direction, RIGHT);
  rs_init(&next_direction, RIGHT);

  rc_init(&score_text, compute_score_text, NULL);
  rc_init(&speed_text, compute_speed_text, NULL); // NEW!
  rc_init(&status_text, compute_status_text, NULL);

  reset_game();
  r_flush();

  float accum = 0.0f; // Movement accumulator

  while (!WindowShouldClose()) {
    float dt = GetFrameTime();

    // Input
    if (IsKeyPressed(KEY_UP) && rs_get(&direction) != DOWN)
      rs_set_force(&next_direction, UP);
    if (IsKeyPressed(KEY_DOWN) && rs_get(&direction) != UP)
      rs_set_force(&next_direction, DOWN);
    if (IsKeyPressed(KEY_LEFT) && rs_get(&direction) != RIGHT)
      rs_set_force(&next_direction, LEFT);
    if (IsKeyPressed(KEY_RIGHT) && rs_get(&direction) != LEFT)
      rs_set_force(&next_direction, RIGHT);

    if (IsKeyPressed(KEY_P))
      rs_set_force(&paused, !rs_get(&paused));
    if (IsKeyPressed(KEY_R))
      reset_game();

    // NEW: Manual speed control (hold for rapid change)
    if (IsKeyDown(KEY_COMMA))
      rs_set_force(&speed, fmaxf(1.0f, rs_get(&speed) - dt * 10.0f));
    if (IsKeyDown(KEY_PERIOD)) {
      float temp = rs_get(&speed) + dt * 10.0f;
      rs_set_force(&speed, temp);
    }

    // Smooth timer-based movement (REACTIVE: uses rs_get(&speed)!)
    accum += dt * rs_get(&speed);
    while (accum >= 1.0f) {
      move_snake();
      accum -= 1.0f;
    }

    // Draw
    BeginDrawing();
    ClearBackground(DARKGREEN);

    // Grid
    for (int y = 0; y < GRID_SIZE; ++y)
      for (int x = 0; x < GRID_SIZE; ++x)
        DrawRectangle(x * CELL_SIZE, y * CELL_SIZE, CELL_SIZE, CELL_SIZE,
                      (x + y) % 2 ? DARKGREEN : (Color){0, 100, 0, 255});

    // Snake
    for (int i = 0; i < snake.count; ++i) {
      Color c = (i == 0) ? LIME : GREEN;
      DrawRectangle(snake.data[i].x * CELL_SIZE + 3,
                    snake.data[i].y * CELL_SIZE + 3, CELL_SIZE - 6,
                    CELL_SIZE - 6, c);
    }

    // Food
    DrawCircle(rs_get(&food).x * CELL_SIZE + CELL_SIZE / 2,
               rs_get(&food).y * CELL_SIZE + CELL_SIZE / 2, CELL_SIZE / 2 - 4,
               RED);

    // UI (NEW: speed display!)
    DrawRectangle(0, GRID_SIZE * CELL_SIZE, SCREEN_W, 100, Fade(BLACK, 0.7f));
    DrawText(rc_get(&score_text), 20, GRID_SIZE * CELL_SIZE + 10, 36, YELLOW);
    DrawText(rc_get(&speed_text), 20, GRID_SIZE * CELL_SIZE + 45, 28, BLUE);
    DrawText(rc_get(&status_text), 20, GRID_SIZE * CELL_SIZE + 75, 20,
             LIGHTGRAY);

    r_flush();
    EndDrawing();
  }

  // Cleanup
  if (snake.data)
    ANO_REACTIVE_FREE(snake.data);
  rc_free(&score_text);
  rc_free(&speed_text);
  rc_free(&status_text);
  rs_free(&score);
  rs_free(&game_over);
  rs_free(&paused);
  rs_free(&speed);
  rs_free(&direction);
  rs_free(&next_direction);
  rs_free(&snake_head);
  rs_free(&food);

  r_shutdown();
  CloseWindow();
  return 0;
}
