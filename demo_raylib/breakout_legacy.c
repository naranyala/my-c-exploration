// Build: cc reactive_breakout.c -o breakout -lraylib -lm
// Requirements: raylib installed. On Linux, also -lGL -lm -lpthread -ldl -lrt -lX11 depending on your setup.

#include <raylib.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include "../lib/reactive.h" // Use your header as provided, but avoid signal_* factories

// -------------------- Config --------------------
#define SCREEN_W 800
#define SCREEN_H 600

#define PADDLE_W 100
#define PADDLE_H 16
#define PADDLE_Y   (SCREEN_H - 60)
#define PADDLE_SPEED 450.0

#define BALL_R 8
#define BALL_SPEED 320.0

#define BRICK_ROWS 6
#define BRICK_COLS 10
#define BRICK_W 70
#define BRICK_H 24
#define BRICK_PAD_X 20
#define BRICK_PAD_Y 60

// -------------------- Signals --------------------
// Note: We define them as globals to ensure valid addresses in the registry.

static Signal s_ball_x;
static Signal s_ball_y;
static Signal s_ball_vx;
static Signal s_ball_vy;

static Signal s_paddle_x;

static Signal s_score;
static Signal s_lives;

static Signal s_game_over; // int: 0 or 1
static Signal s_paused;    // int: 0 or 1

// Brick signals: alive per brick
static Signal s_brick_alive[BRICK_ROWS][BRICK_COLS];

// Computed signals (structure holders)
static Signal s_collide_walls;
static Signal s_collide_paddle;
static Signal s_collide_bricks;

// -------------------- Helpers --------------------
static Rectangle PaddleRect(double px) {
    return (Rectangle){ (float)px - PADDLE_W/2.0f, (float)PADDLE_Y, (float)PADDLE_W, (float)PADDLE_H };
}

static Rectangle BrickRect(int r, int c) {
    float x0 = (SCREEN_W - (BRICK_COLS * BRICK_W)) / 2.0f;
    return (Rectangle){ x0 + c * BRICK_W, BRICK_PAD_Y + r * BRICK_H, BRICK_W - 2, BRICK_H - 2 };
}

static bool CircleRectOverlap(double cx, double cy, double r, Rectangle rect) {
    double closestX = fmax(rect.x, fmin(cx, rect.x + rect.width));
    double closestY = fmax(rect.y, fmin(cy, rect.y + rect.height));
    double dx = cx - closestX;
    double dy = cy - closestY;
    return (dx*dx + dy*dy) <= r*r;
}

// -------------------- Computed: walls --------------------
static void compute_collide_walls(Signal *self) {
    double x = get_double(&s_ball_x);
    double y = get_double(&s_ball_y);
    double vx = get_double(&s_ball_vx);
    double vy = get_double(&s_ball_vy);

    bool bounced = false;

    // Left-right walls
    if (x - BALL_R <= 0 && vx < 0) { set_double(&s_ball_vx, -vx); bounced = true; }
    if (x + BALL_R >= SCREEN_W && vx > 0) { set_double(&s_ball_vx, -vx); bounced = true; }

    // Top wall
    if (y - BALL_R <= 0 && vy < 0) { set_double(&s_ball_vy, -vy); bounced = true; }

    // Bottom: lose life
    if (y - BALL_R > SCREEN_H) {
        int lives = get_int(&s_lives);
        if (lives > 0) {
            set_int(&s_lives, lives - 1);
            // Reset ball to center above paddle
            set_double(&s_ball_x, get_double(&s_paddle_x));
            set_double(&s_ball_y, PADDLE_Y - 30);
            set_double(&s_ball_vx, BALL_SPEED * 0.7);
            set_double(&s_ball_vy, -BALL_SPEED);
        } else {
            set_int(&s_game_over, 1);
        }
    }

    self->dirty = bounced;
}

// -------------------- Computed: paddle --------------------
static void compute_collide_paddle(Signal *self) {
    Rectangle paddle = PaddleRect(get_double(&s_paddle_x));
    double x = get_double(&s_ball_x);
    double y = get_double(&s_ball_y);
    double vx = get_double(&s_ball_vx);
    double vy = get_double(&s_ball_vy);

    if (CircleRectOverlap(x, y, BALL_R, paddle) && vy > 0) {
        // Reflect Y
        set_double(&s_ball_vy, -fabs(vy));

        // Add some x based on hit position
        double center = paddle.x + paddle.width / 2.0;
        double offset = (x - center) / (paddle.width / 2.0); // -1..1
        double new_vx = BALL_SPEED * 0.9 * offset;
        set_double(&s_ball_vx, new_vx);

        // Nudge ball above paddle to avoid sticking
        set_double(&s_ball_y, paddle.y - BALL_R - 0.1);
    }

    self->dirty = false;
}

// -------------------- Computed: bricks --------------------
static void compute_collide_bricks(Signal *self) {
    double x = get_double(&s_ball_x);
    double y = get_double(&s_ball_y);
    double vx = get_double(&s_ball_vx);
    double vy = get_double(&s_ball_vy);

    bool hit_any = false;

    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (get_int(&s_brick_alive[r][c]) == 0) continue;

            Rectangle br = BrickRect(r, c);
            if (CircleRectOverlap(x, y, BALL_R, br)) {
                // Decide vertical vs horizontal bounce using overlap extents
                double left = br.x, right = br.x + br.width;
                double top = br.y, bottom = br.y + br.height;

                double cx = x, cy = y;
                double closestX = fmax(left, fmin(cx, right));
                double closestY = fmax(top, fmin(cy, bottom));
                double dx = cx - closestX;
                double dy = cy - closestY;

                // Prefer flipping axis with greater penetration
                if (fabs(dx) > fabs(dy)) {
                    set_double(&s_ball_vx, -vx);
                } else {
                    set_double(&s_ball_vy, -vy);
                }

                set_int(&s_brick_alive[r][c], 0);
                set_int(&s_score, get_int(&s_score) + 10);
                hit_any = true;
                // Exit early per frame to avoid multiple flips
                goto done;
            }
        }
    }

done:
    self->dirty = hit_any;
}

// -------------------- Initialization helpers --------------------
static void init_signal_int(Signal *s, int value) {
    memset(s, 0, sizeof(*s));
    s->type = SIG_INT;
    s->val.i = value;
    register_signal(s);
}

static void init_signal_double(Signal *s, double value) {
    memset(s, 0, sizeof(*s));
    s->type = SIG_DOUBLE;
    s->val.d = value;
    register_signal(s);
}

static void init_computed(Signal *s, void (*compute)(Signal *), Signal **deps, int dep_count) {
    memset(s, 0, sizeof(*s));
    s->compute = compute;
    s->dep_count = dep_count > MAX_DEPS ? MAX_DEPS : dep_count;
    for (int i = 0; i < s->dep_count; i++) s->deps[i] = deps[i];
    register_signal(s);
    compute(s);
    s->dirty = false;
}

// -------------------- Setup --------------------
static void SetupGame(void) {
    // Main signals
    init_signal_double(&s_ball_x, SCREEN_W / 2.0);
    init_signal_double(&s_ball_y, SCREEN_H / 2.0);
    init_signal_double(&s_ball_vx, BALL_SPEED * 0.7);
    init_signal_double(&s_ball_vy, -BALL_SPEED);

    init_signal_double(&s_paddle_x, SCREEN_W / 2.0);

    init_signal_int(&s_score, 0);
    init_signal_int(&s_lives, 3);
    init_signal_int(&s_game_over, 0);
    init_signal_int(&s_paused, 0);

    // Bricks
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            init_signal_int(&s_brick_alive[r][c], 1);
        }
    }

    // Computed dependencies
    Signal *deps_walls[]  = { &s_ball_x, &s_ball_y, &s_ball_vx, &s_ball_vy, &s_lives };
    init_computed(&s_collide_walls, compute_collide_walls, deps_walls, sizeof(deps_walls)/sizeof(deps_walls[0]));

    Signal *deps_paddle[] = { &s_ball_x, &s_ball_y, &s_ball_vx, &s_ball_vy, &s_paddle_x };
    init_computed(&s_collide_paddle, compute_collide_paddle, deps_paddle, sizeof(deps_paddle)/sizeof(deps_paddle[0]));

    // For bricks, we depend on ball position + every brick alive flag
    Signal *deps_bricks[MAX_DEPS];
    int dep_count = 0;
    deps_bricks[dep_count++] = &s_ball_x;
    deps_bricks[dep_count++] = &s_ball_y;
    // Note: MAX_DEPS is 8, so we cannot include all brick signals here (there are many).
    // Instead, we will trigger bricks compute manually when ball moves (which happens via set_*).
    init_computed(&s_collide_bricks, compute_collide_bricks, deps_bricks, dep_count);
}

// -------------------- Update --------------------
static void UpdateGame(double dt) {
    if (get_int(&s_game_over)) return;

    // Toggle pause
    if (IsKeyPressed(KEY_P)) {
        set_int(&s_paused, !get_int(&s_paused));
    }
    if (get_int(&s_paused)) return;

    // Paddle input
    double px = get_double(&s_paddle_x);
    if (IsKeyDown(KEY_LEFT))  px -= PADDLE_SPEED * dt;
    if (IsKeyDown(KEY_RIGHT)) px += PADDLE_SPEED * dt;
    if (px < PADDLE_W/2.0) px = PADDLE_W/2.0;
    if (px > SCREEN_W - PADDLE_W/2.0) px = SCREEN_W - PADDLE_W/2.0;
    set_double(&s_paddle_x, px);

    // Ball movement
    double x = get_double(&s_ball_x);
    double y = get_double(&s_ball_y);
    double vx = get_double(&s_ball_vx);
    double vy = get_double(&s_ball_vy);

    x += vx * dt;
    y += vy * dt;

    set_double(&s_ball_x, x);
    set_double(&s_ball_y, y);

    // Manually compute brick collisions since MAX_DEPS limits full dependency list
    s_collide_bricks.compute(&s_collide_bricks);
}

// -------------------- Draw --------------------
static void DrawGame(void) {
    BeginDrawing();
    ClearBackground((Color){18, 18, 22, 255});

    // UI
    DrawText(TextFormat("Score: %d", get_int(&s_score)), 20, 10, 20, RAYWHITE);
    DrawText(TextFormat("Lives: %d", get_int(&s_lives)), SCREEN_W - 140, 10, 20, RAYWHITE);
    if (get_int(&s_paused)) DrawText("Paused (P)", SCREEN_W/2 - 60, 10, 20, YELLOW);

    // Paddle
    Rectangle paddle = PaddleRect(get_double(&s_paddle_x));
    DrawRectangleRec(paddle, (Color){80, 210, 120, 255});

    // Ball
    DrawCircle((int)get_double(&s_ball_x), (int)get_double(&s_ball_y), BALL_R, (Color){220, 80, 90, 255});

    // Bricks
    for (int r = 0; r < BRICK_ROWS; r++) {
        for (int c = 0; c < BRICK_COLS; c++) {
            if (get_int(&s_brick_alive[r][c]) == 0) continue;
            Rectangle br = BrickRect(r, c);
            Color col = (Color){ (unsigned char)(120 + r*20), (unsigned char)(160 - r*15), (unsigned char)(200 - c*5), 255 };
            DrawRectangleRec(br, col);
        }
    }

    // Game over
    if (get_int(&s_game_over)) {
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, (Color){0,0,0,150});
        DrawText("Game Over", SCREEN_W/2 - 120, SCREEN_H/2 - 40, 40, RED);
        DrawText(TextFormat("Final Score: %d", get_int(&s_score)), SCREEN_W/2 - 120, SCREEN_H/2 + 10, 24, RAYWHITE);
        DrawText("Press R to Restart", SCREEN_W/2 - 120, SCREEN_H/2 + 42, 18, YELLOW);
    }

    EndDrawing();
}

// -------------------- Reset / main --------------------
static void ResetBricks(void) {
    for (int r = 0; r < BRICK_ROWS; r++)
        for (int c = 0; c < BRICK_COLS; c++)
            set_int(&s_brick_alive[r][c], 1);
}

static void ResetBallPaddle(void) {
    set_double(&s_paddle_x, SCREEN_W / 2.0);
    set_double(&s_ball_x, get_double(&s_paddle_x));
    set_double(&s_ball_y, PADDLE_Y - 30);
    set_double(&s_ball_vx, BALL_SPEED * 0.7);
    set_double(&s_ball_vy, -BALL_SPEED);
}

int main(void) {
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(SCREEN_W, SCREEN_H, "Breakout (Reactive + Raylib)");
    SetTargetFPS(60);

    SetupGame();
    ResetBallPaddle();

    while (!WindowShouldClose()) {
        double dt = GetFrameTime();

        if (get_int(&s_game_over)) {
            if (IsKeyPressed(KEY_R)) {
                set_int(&s_score, 0);
                set_int(&s_lives, 3);
                set_int(&s_game_over, 0);
                ResetBricks();
                ResetBallPaddle();
            }
        }

        UpdateGame(dt);
        DrawGame();
    }

    CloseWindow();
    return 0;
}

