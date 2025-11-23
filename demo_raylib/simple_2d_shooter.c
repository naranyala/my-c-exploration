// Compile with: gcc game.c -o game -lraylib -lm
#include "raylib.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ENEMIES 10
#define ENEMY_SPAWN_TIME 2.0f
#define ENEMY_SPEED 50.0f

typedef struct {
    Vector2 position;
    float radius;
    bool active;
    Color color;
} Enemy;

typedef struct {
    Vector2 position;
    Vector2 velocity;
    float lifetime;
    bool active;
} Bullet;

typedef struct {
    Enemy enemies[MAX_ENEMIES];
    Bullet bullets[100];
    int score;
    int lives;
    float spawnTimer;
    bool gameOver;
} GameState;

void InitGame(GameState *game) {
    game->score = 0;
    game->lives = 3;
    game->spawnTimer = 0.0f;
    game->gameOver = false;
    
    for (int i = 0; i < MAX_ENEMIES; i++) {
        game->enemies[i].active = false;
    }
    
    for (int i = 0; i < 100; i++) {
        game->bullets[i].active = false;
    }
}

void SpawnEnemy(GameState *game, int screenWidth, int screenHeight) {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (!game->enemies[i].active) {
            Enemy *e = &game->enemies[i];
            e->active = true;
            e->radius = 20.0f + GetRandomValue(0, 20);
            
            // Spawn from random edge
            int edge = GetRandomValue(0, 3);
            switch (edge) {
                case 0: // Top
                    e->position.x = GetRandomValue(0, screenWidth);
                    e->position.y = -e->radius;
                    break;
                case 1: // Right
                    e->position.x = screenWidth + e->radius;
                    e->position.y = GetRandomValue(0, screenHeight);
                    break;
                case 2: // Bottom
                    e->position.x = GetRandomValue(0, screenWidth);
                    e->position.y = screenHeight + e->radius;
                    break;
                case 3: // Left
                    e->position.x = -e->radius;
                    e->position.y = GetRandomValue(0, screenHeight);
                    break;
            }
            
            e->color = (Color){
                GetRandomValue(150, 255),
                GetRandomValue(50, 100),
                GetRandomValue(50, 100),
                255
            };
            break;
        }
    }
}

void ShootBullet(GameState *game, Vector2 from, Vector2 to) {
    for (int i = 0; i < 100; i++) {
        if (!game->bullets[i].active) {
            Bullet *b = &game->bullets[i];
            b->active = true;
            b->position = from;
            b->lifetime = 3.0f;
            
            // Calculate direction
            Vector2 dir = {to.x - from.x, to.y - from.y};
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            if (len > 0) {
                dir.x /= len;
                dir.y /= len;
            }
            
            b->velocity.x = dir.x * 800.0f;
            b->velocity.y = dir.y * 800.0f;
            break;
        }
    }
}

void UpdateGame(GameState *game, int screenWidth, int screenHeight, float dt) {
    if (game->gameOver) return;
    
    Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};
    
    // Spawn enemies
    game->spawnTimer += dt;
    if (game->spawnTimer >= ENEMY_SPAWN_TIME) {
        SpawnEnemy(game, screenWidth, screenHeight);
        game->spawnTimer = 0.0f;
    }
    
    // Update enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            Enemy *e = &game->enemies[i];
            
            // Move toward center
            Vector2 dir = {center.x - e->position.x, center.y - e->position.y};
            float len = sqrtf(dir.x * dir.x + dir.y * dir.y);
            
            if (len < e->radius) {
                // Enemy reached center - lose life
                e->active = false;
                game->lives--;
                if (game->lives <= 0) {
                    game->gameOver = true;
                }
            } else if (len > 0) {
                dir.x /= len;
                dir.y /= len;
                e->position.x += dir.x * ENEMY_SPEED * dt;
                e->position.y += dir.y * ENEMY_SPEED * dt;
            }
        }
    }
    
    // Update bullets
    for (int i = 0; i < 100; i++) {
        if (game->bullets[i].active) {
            Bullet *b = &game->bullets[i];
            b->position.x += b->velocity.x * dt;
            b->position.y += b->velocity.y * dt;
            b->lifetime -= dt;
            
            if (b->lifetime <= 0 || 
                b->position.x < -50 || b->position.x > screenWidth + 50 ||
                b->position.y < -50 || b->position.y > screenHeight + 50) {
                b->active = false;
            }
            
            // Check bullet-enemy collision
            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (game->enemies[j].active) {
                    Enemy *e = &game->enemies[j];
                    float dx = b->position.x - e->position.x;
                    float dy = b->position.y - e->position.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    
                    if (dist < e->radius) {
                        // Hit!
                        e->active = false;
                        b->active = false;
                        game->score += (int)(10 * (e->radius / 20.0f));
                        break;
                    }
                }
            }
        }
    }
}

void DrawGame(GameState *game, int screenWidth, int screenHeight) {
    Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};
    
    // Draw background grid
    for (int i = 0; i < screenWidth; i += 50) {
        DrawLine(i, 0, i, screenHeight, Fade(LIGHTGRAY, 0.3f));
    }
    for (int i = 0; i < screenHeight; i += 50) {
        DrawLine(0, i, screenWidth, i, Fade(LIGHTGRAY, 0.3f));
    }
    
    // Draw center circle (player area)
    DrawCircleV(center, 40, Fade(SKYBLUE, 0.3f));
    DrawCircleLinesV(center, 40, SKYBLUE);
    
    // Draw enemies
    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (game->enemies[i].active) {
            Enemy *e = &game->enemies[i];
            DrawCircleV(e->position, e->radius, e->color);
            DrawCircleLinesV(e->position, e->radius, BLACK);
            
            // Draw line to center
            DrawLineEx(e->position, center, 1.0f, Fade(RED, 0.3f));
        }
    }
    
    // Draw bullets
    for (int i = 0; i < 100; i++) {
        if (game->bullets[i].active) {
            DrawCircleV(game->bullets[i].position, 4.0f, YELLOW);
            DrawCircleLinesV(game->bullets[i].position, 4.0f, ORANGE);
        }
    }
    
    // Draw crosshair at center
    DrawCircleLinesV(center, 8, RED);
    DrawLineEx((Vector2){center.x - 15, center.y}, 
               (Vector2){center.x - 5, center.y}, 2.0f, RED);
    DrawLineEx((Vector2){center.x + 5, center.y}, 
               (Vector2){center.x + 15, center.y}, 2.0f, RED);
    DrawLineEx((Vector2){center.x, center.y - 15}, 
               (Vector2){center.x, center.y - 5}, 2.0f, RED);
    DrawLineEx((Vector2){center.x, center.y + 5}, 
               (Vector2){center.x, center.y + 15}, 2.0f, RED);
    
    // Draw mouse cursor line
    Vector2 mousePos = GetMousePosition();
    DrawLineEx(center, mousePos, 1.0f, Fade(GREEN, 0.5f));
    DrawCircleLinesV(mousePos, 5, GREEN);
    
    // Draw UI
    DrawText(TextFormat("SCORE: %d", game->score), 10, 10, 30, BLACK);
    DrawText(TextFormat("LIVES: %d", game->lives), 10, 50, 30, 
             game->lives > 1 ? BLACK : RED);
    
    DrawText("LEFT CLICK to shoot", 10, screenHeight - 30, 20, DARKGRAY);
    
    if (game->gameOver) {
        int textWidth = MeasureText("GAME OVER!", 60);
        DrawRectangle(0, screenHeight/2 - 80, screenWidth, 160, Fade(BLACK, 0.8f));
        DrawText("GAME OVER!", screenWidth/2 - textWidth/2, screenHeight/2 - 30, 60, RED);
        DrawText(TextFormat("Final Score: %d", game->score), 
                 screenWidth/2 - 100, screenHeight/2 + 40, 30, WHITE);
        DrawText("Press R to restart", screenWidth/2 - 100, screenHeight/2 + 80, 20, LIGHTGRAY);
    }
}

int main(void) {
    const int screenWidth = 1200;
    const int screenHeight = 800;
    
    InitWindow(screenWidth, screenHeight, "Simple 2D Shooter");
    SetTargetFPS(60);
    
    srand(time(NULL));
    
    GameState game;
    InitGame(&game);
    
    Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        
        // Input
        if (game.gameOver && IsKeyPressed(KEY_R)) {
            InitGame(&game);
        }
        
        if (!game.gameOver && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Vector2 mousePos = GetMousePosition();
            ShootBullet(&game, center, mousePos);
        }
        
        // Update
        UpdateGame(&game, screenWidth, screenHeight, dt);
        
        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        DrawGame(&game, screenWidth, screenHeight);
        
        DrawFPS(screenWidth - 100, 10);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
