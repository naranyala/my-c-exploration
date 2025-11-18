#include "raylib.h"
#include "reactive.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 20
#define CELL_SIZE 30
#define MAX_SNAKE 400

typedef struct {
    int x, y;
} Point;

// Game state signals
Signal snake_length;
Signal score;
Signal game_over;
Signal direction;  // 0=right, 1=down, 2=left, 3=up
Signal food_x;
Signal food_y;

// Snake body (not reactive, but updated based on signals)
Point snake[MAX_SNAKE];

// Computed display text
Signal score_text;

// ----- Computed Score Text -----
void compute_score_text(Signal *self) {
    char buf[64];
    snprintf(buf, sizeof(buf), "Score: %d", get_int(self->deps[0]));
    set_string(self, buf);
}

// ----- Game Logic -----
void spawn_food() {
    int fx = rand() % GRID_SIZE;
    int fy = rand() % GRID_SIZE;
    
    // Make sure food doesn't spawn on snake
    int valid = 1;
    int len = get_int(&snake_length);
    for (int i = 0; i < len; i++) {
        if (snake[i].x == fx && snake[i].y == fy) {
            valid = 0;
            break;
        }
    }
    
    if (valid) {
        set_int(&food_x, fx);
        set_int(&food_y, fy);
    } else {
        spawn_food(); // Try again
    }
}

void init_game() {
    // Initialize snake in middle
    snake[0].x = GRID_SIZE / 2;
    snake[0].y = GRID_SIZE / 2;
    
    set_int(&snake_length, 1);
    set_int(&score, 0);
    set_int(&game_over, 0);
    set_int(&direction, 0);  // Start moving right
    
    spawn_food();
}

void update_game() {
    if (get_int(&game_over)) return;
    
    int len = get_int(&snake_length);
    int dir = get_int(&direction);
    
    // Move body
    for (int i = len - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    
    // Move head based on direction
    switch (dir) {
        case 0: snake[0].x++; break;  // right
        case 1: snake[0].y++; break;  // down
        case 2: snake[0].x--; break;  // left
        case 3: snake[0].y--; break;  // up
    }
    
    // Check wall collision
    if (snake[0].x < 0 || snake[0].x >= GRID_SIZE ||
        snake[0].y < 0 || snake[0].y >= GRID_SIZE) {
        set_int(&game_over, 1);
        return;
    }
    
    // Check self collision
    for (int i = 1; i < len; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            set_int(&game_over, 1);
            return;
        }
    }
    
    // Check food collision
    if (snake[0].x == get_int(&food_x) && snake[0].y == get_int(&food_y)) {
        set_int(&snake_length, len + 1);
        set_int(&score, get_int(&score) + 10);
        spawn_food();
    }
}

void handle_input() {
    if (get_int(&game_over)) {
        if (IsKeyPressed(KEY_SPACE)) {
            init_game();
        }
        return;
    }
    
    int current_dir = get_int(&direction);
    
    // Prevent 180 degree turns
    if (IsKeyPressed(KEY_RIGHT) && current_dir != 2) {
        set_int(&direction, 0);
    }
    else if (IsKeyPressed(KEY_DOWN) && current_dir != 3) {
        set_int(&direction, 1);
    }
    else if (IsKeyPressed(KEY_LEFT) && current_dir != 0) {
        set_int(&direction, 2);
    }
    else if (IsKeyPressed(KEY_UP) && current_dir != 1) {
        set_int(&direction, 3);
    }
}

void draw_game() {
    ClearBackground(DARKGREEN);
    
    // Draw grid
    for (int i = 0; i <= GRID_SIZE; i++) {
        DrawLine(0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE, i * CELL_SIZE, 
                 Fade(GREEN, 0.3f));
        DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, GRID_SIZE * CELL_SIZE, 
                 Fade(GREEN, 0.3f));
    }
    
    // Draw food
    int fx = get_int(&food_x);
    int fy = get_int(&food_y);
    DrawRectangle(fx * CELL_SIZE + 2, fy * CELL_SIZE + 2, 
                  CELL_SIZE - 4, CELL_SIZE - 4, RED);
    
    // Draw snake
    int len = get_int(&snake_length);
    for (int i = 0; i < len; i++) {
        Color c = (i == 0) ? YELLOW : LIME;
        DrawRectangle(snake[i].x * CELL_SIZE + 2, 
                      snake[i].y * CELL_SIZE + 2,
                      CELL_SIZE - 4, CELL_SIZE - 4, c);
    }
    
    // Draw score (automatically updated via reactive system!)
    DrawText(get_string(&score_text), 10, GRID_SIZE * CELL_SIZE + 10, 30, WHITE);
    
    // Draw game over
    if (get_int(&game_over)) {
        DrawRectangle(0, 0, GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE, 
                      Fade(BLACK, 0.7f));
        const char *msg = "GAME OVER!";
        int tw = MeasureText(msg, 50);
        DrawText(msg, (GRID_SIZE * CELL_SIZE - tw) / 2, 
                 GRID_SIZE * CELL_SIZE / 2 - 50, 50, RED);
        
        const char *restart = "Press SPACE to restart";
        int tw2 = MeasureText(restart, 20);
        DrawText(restart, (GRID_SIZE * CELL_SIZE - tw2) / 2,
                 GRID_SIZE * CELL_SIZE / 2 + 20, 20, WHITE);
    }
}

int main() {
    srand(time(NULL));
    
    InitWindow(GRID_SIZE * CELL_SIZE, GRID_SIZE * CELL_SIZE + 50, 
               "Reactive Snake Game");
    SetTargetFPS(60);
    
    // Initialize signals
    snake_length = signal_int(1);
    score = signal_int(0);
    game_over = signal_int(0);
    direction = signal_int(0);
    food_x = signal_int(5);
    food_y = signal_int(5);
    
    // Register signals
    register_signal(&snake_length);
    register_signal(&score);
    register_signal(&game_over);
    register_signal(&direction);
    register_signal(&food_x);
    register_signal(&food_y);
    
    // Create computed score text
    Signal *deps[] = {&score};
    score_text = signal_computed(compute_score_text, deps, 1);
    register_signal(&score_text);
    
    // Initialize game
    init_game();
    
    float move_timer = 0.0f;
    float move_speed = 0.15f;  // Move every 0.15 seconds
    
    while (!WindowShouldClose()) {
        // Handle input
        handle_input();
        
        // Update game at fixed intervals
        move_timer += GetFrameTime();
        if (move_timer >= move_speed) {
            move_timer = 0.0f;
            update_game();
        }
        
        // Draw
        BeginDrawing();
        draw_game();
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
