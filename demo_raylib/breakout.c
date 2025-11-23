// breakout.c - Compile: gcc breakout.c -o breakout -lraylib -lm
#include "raylib.h"
#include <stdlib.h>
#include <math.h>

// Include your reactive library
#include "../lib/reactive.h"

// ===== Game Constants =====
#define SCREEN_W 800
#define SCREEN_H 600
#define PADDLE_W 100
#define PADDLE_H 15
#define BALL_R 8
#define BRICK_ROWS 5
#define BRICK_COLS 10
#define BRICK_W 70
#define BRICK_H 25
#define BRICK_PAD 5

// ===== Brick Structure =====
typedef struct {
    Signal *alive;
    Rectangle rect;
    Color color;
} Brick;

// ===== Game State (Reactive Signals) =====
Signal *paddle_x;
Signal *ball_x, *ball_y;
Signal *ball_vx, *ball_vy;
Signal *score;
Signal *lives;
Signal *game_over;

Brick bricks[BRICK_ROWS * BRICK_COLS];
int brick_count;

// ===== Initialize Game =====
void init_game(void) {
    // Create reactive signals for game state
    paddle_x = signal_double(SCREEN_W / 2.0 - PADDLE_W / 2.0);
    
    ball_x = signal_double(SCREEN_W / 2.0);
    ball_y = signal_double(SCREEN_H - 80);
    ball_vx = signal_double(4.0);
    ball_vy = signal_double(-4.0);
    
    score = signal_int(0);
    lives = signal_int(3);
    game_over = signal_int(0);
    
    // Initialize bricks
    brick_count = BRICK_ROWS * BRICK_COLS;
    Color colors[] = {RED, ORANGE, YELLOW, GREEN, BLUE};
    
    for (int row = 0; row < BRICK_ROWS; row++) {
        for (int col = 0; col < BRICK_COLS; col++) {
            int idx = row * BRICK_COLS + col;
            float x = col * (BRICK_W + BRICK_PAD) + 35;
            float y = row * (BRICK_H + BRICK_PAD) + 50;
            
            bricks[idx].alive = signal_int(1);
            bricks[idx].rect = (Rectangle){x, y, BRICK_W, BRICK_H};
            bricks[idx].color = colors[row % 5];
        }
    }
}

// ===== Reset Ball =====
void reset_ball(void) {
    set_double(ball_x, SCREEN_W / 2.0);
    set_double(ball_y, SCREEN_H - 80);
    set_double(ball_vx, (GetRandomValue(0, 1) ? 4.0 : -4.0));
    set_double(ball_vy, -4.0);
}

// ===== Update Game Logic =====
void update_game(void) {
    if (get_int(game_over)) return;
    
    // Paddle follows mouse
    float mx = GetMouseX() - PADDLE_W / 2.0;
    if (mx < 0) mx = 0;
    if (mx > SCREEN_W - PADDLE_W) mx = SCREEN_W - PADDLE_W;
    set_double(paddle_x, mx);
    
    // Ball movement
    double bx = get_double(ball_x) + get_double(ball_vx);
    double by = get_double(ball_y) + get_double(ball_vy);
    double vx = get_double(ball_vx);
    double vy = get_double(ball_vy);
    
    // Wall collisions
    if (bx <= BALL_R || bx >= SCREEN_W - BALL_R) {
        vx = -vx;
        bx = (bx <= BALL_R) ? BALL_R : SCREEN_W - BALL_R;
    }
    if (by <= BALL_R) {
        vy = -vy;
        by = BALL_R;
    }
    
    // Bottom - lose life
    if (by >= SCREEN_H) {
        set_int(lives, get_int(lives) - 1);
        if (get_int(lives) <= 0) {
            set_int(game_over, 1);
        } else {
            reset_ball();
            return;
        }
    }
    
    // Paddle collision
    Rectangle paddle = {get_double(paddle_x), SCREEN_H - 40, PADDLE_W, PADDLE_H};
    Vector2 ball_center = {bx, by};
    
    if (CheckCollisionCircleRec(ball_center, BALL_R, paddle) && vy > 0) {
        vy = -fabs(vy);
        // Angle based on where ball hits paddle
        float hit_pos = (bx - get_double(paddle_x)) / PADDLE_W;
        vx = (hit_pos - 0.5f) * 8.0f;
        by = paddle.y - BALL_R;
    }
    
    // Brick collisions
    for (int i = 0; i < brick_count; i++) {
        if (!get_int(bricks[i].alive)) continue;
        
        if (CheckCollisionCircleRec(ball_center, BALL_R, bricks[i].rect)) {
            set_int(bricks[i].alive, 0);  // Reactive: brick dies
            set_int(score, get_int(score) + 10);  // Reactive: score updates
            
            // Determine bounce direction
            float cx = bricks[i].rect.x + bricks[i].rect.width / 2;
            float cy = bricks[i].rect.y + bricks[i].rect.height / 2;
            float dx = bx - cx;
            float dy = by - cy;
            
            if (fabs(dx) > fabs(dy)) vx = -vx;
            else vy = -vy;
            
            break;
        }
    }
    
    // Check win condition
    int alive_count = 0;
    for (int i = 0; i < brick_count; i++) {
        if (get_int(bricks[i].alive)) alive_count++;
    }
    if (alive_count == 0) set_int(game_over, 2);  // Win!
    
    // Apply updates via reactive system
    set_double(ball_vx, vx);
    set_double(ball_vy, vy);
    set_double(ball_x, bx);
    set_double(ball_y, by);
}

// ===== Draw Game =====
void draw_game(void) {
    BeginDrawing();
    ClearBackground((Color){20, 20, 40, 255});
    
    // Draw bricks (reactive: only alive ones)
    for (int i = 0; i < brick_count; i++) {
        if (get_int(bricks[i].alive)) {
            DrawRectangleRec(bricks[i].rect, bricks[i].color);
            DrawRectangleLinesEx(bricks[i].rect, 2, WHITE);
        }
    }
    
    // Draw paddle
    DrawRectangle(get_double(paddle_x), SCREEN_H - 40, PADDLE_W, PADDLE_H, WHITE);
    
    // Draw ball
    DrawCircle(get_double(ball_x), get_double(ball_y), BALL_R, RAYWHITE);
    
    // HUD
    DrawText(TextFormat("Score: %d", get_int(score)), 10, 10, 20, WHITE);
    DrawText(TextFormat("Lives: %d", get_int(lives)), SCREEN_W - 100, 10, 20, WHITE);
    
    // Game over / win message
    if (get_int(game_over) == 1) {
        DrawText("GAME OVER", SCREEN_W/2 - 100, SCREEN_H/2, 40, RED);
        DrawText("Press R to restart", SCREEN_W/2 - 90, SCREEN_H/2 + 50, 20, WHITE);
    } else if (get_int(game_over) == 2) {
        DrawText("YOU WIN!", SCREEN_W/2 - 80, SCREEN_H/2, 40, GREEN);
        DrawText("Press R to restart", SCREEN_W/2 - 90, SCREEN_H/2 + 50, 20, WHITE);
    }
    
    EndDrawing();
}

// ===== Main =====
int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Breakout - Reactive Edition");
    SetTargetFPS(60);
    HideCursor();
    
    init_game();
    
    while (!WindowShouldClose()) {
        // Restart game
        if (IsKeyPressed(KEY_R)) {
            g_signal_count = 0;  // Reset reactive registry
            init_game();
        }
        
        update_game();
        draw_game();
    }
    
    CloseWindow();
    return 0;
}
