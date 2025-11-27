#include "raylib.h"
#include <stdlib.h>
#include <time.h>

#define MAX_ENEMIES 24
#define MAX_ENEMY_BULLETS 5
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

typedef struct {
    Rectangle rect;
    bool active;
    float speed;
} Bullet;

typedef struct {
    Rectangle rect;
    bool active;
    int type; // 0=normal, 1=faster, 2=tougher
    int health;
} Enemy;

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Space Invaders - Enhanced Edition");
    InitAudioDevice();
    
    srand(time(NULL));

    // Player
    Rectangle player = {380, 550, 40, 10};
    int playerLives = 3;
    bool invincible = false;
    float invincibleTimer = 0;
    
    // Player bullets
    Bullet bullets[3] = {0};
    for (int i = 0; i < 3; i++) {
        bullets[i].rect = (Rectangle){0, 0, 4, 10};
        bullets[i].active = false;
        bullets[i].speed = 8;
    }

    // Enemy bullets
    Bullet enemyBullets[MAX_ENEMY_BULLETS] = {0};
    for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
        enemyBullets[i].rect = (Rectangle){0, 0, 3, 8};
        enemyBullets[i].active = false;
        enemyBullets[i].speed = 4;
    }
    float enemyShootTimer = 0;

    // Enemies - 3 rows
    Enemy enemies[MAX_ENEMIES];
    int enemyCount = 0;
    for (int row = 0; row < 3; row++) {
        for (int col = 0; col < 8; col++) {
            enemies[enemyCount].rect = (Rectangle){50 + col*85, 60 + row*50, 30, 20};
            enemies[enemyCount].active = true;
            enemies[enemyCount].type = row; // Different types per row
            enemies[enemyCount].health = (row == 2) ? 2 : 1; // Bottom row has more health
            enemyCount++;
        }
    }

    int alive = MAX_ENEMIES;
    float enemySpeed = 1.5f;
    int dir = 1;
    int score = 0;
    int level = 1;
    bool gameOver = false;
    bool victory = false;

    // Shields
    Rectangle shields[4];
    int shieldHealth[4];
    for (int i = 0; i < 4; i++) {
        shields[i] = (Rectangle){100 + i*200, 480, 60, 30};
        shieldHealth[i] = 3;
    }

    // Power-up
    Rectangle powerUp = {0, 0, 20, 20};
    bool powerUpActive = false;
    float powerUpTimer = 0;
    bool rapidFire = false;
    float rapidFireTimer = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        float delta = GetFrameTime();

        if (!gameOver && !victory) {
            // Invincibility timer
            if (invincible) {
                invincibleTimer -= delta;
                if (invincibleTimer <= 0) invincible = false;
            }

            // Rapid fire timer
            if (rapidFire) {
                rapidFireTimer -= delta;
                if (rapidFireTimer <= 0) rapidFire = false;
            }

            // Player movement
            if (IsKeyDown(KEY_LEFT))  player.x -= 5;
            if (IsKeyDown(KEY_RIGHT)) player.x += 5;
            if (player.x < 0) player.x = 0;
            if (player.x > SCREEN_WIDTH - player.width) player.x = SCREEN_WIDTH - player.width;

            // Fire bullets
            if (IsKeyPressed(KEY_SPACE) || (rapidFire && IsKeyDown(KEY_SPACE))) {
                for (int i = 0; i < 3; i++) {
                    if (!bullets[i].active) {
                        bullets[i].active = true;
                        bullets[i].rect.x = player.x + player.width/2 - bullets[i].rect.width/2;
                        bullets[i].rect.y = player.y;
                        break;
                    }
                }
            }

            // Update player bullets
            for (int i = 0; i < 3; i++) {
                if (bullets[i].active) {
                    bullets[i].rect.y -= bullets[i].speed;
                    if (bullets[i].rect.y < 0) bullets[i].active = false;
                }
            }

            // Enemy shooting
            enemyShootTimer += delta;
            if (enemyShootTimer > 1.5f) {
                enemyShootTimer = 0;
                // Random enemy shoots
                int attempts = 0;
                while (attempts < 20) {
                    int idx = rand() % MAX_ENEMIES;
                    if (enemies[idx].active) {
                        for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                            if (!enemyBullets[i].active) {
                                enemyBullets[i].active = true;
                                enemyBullets[i].rect.x = enemies[idx].rect.x + enemies[idx].rect.width/2;
                                enemyBullets[i].rect.y = enemies[idx].rect.y + enemies[idx].rect.height;
                                break;
                            }
                        }
                        break;
                    }
                    attempts++;
                }
            }

            // Update enemy bullets
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) {
                if (enemyBullets[i].active) {
                    enemyBullets[i].rect.y += enemyBullets[i].speed;
                    if (enemyBullets[i].rect.y > SCREEN_HEIGHT) 
                        enemyBullets[i].active = false;

                    // Hit player
                    if (!invincible && CheckCollisionRecs(enemyBullets[i].rect, player)) {
                        enemyBullets[i].active = false;
                        playerLives--;
                        invincible = true;
                        invincibleTimer = 2.0f;
                        if (playerLives <= 0) gameOver = true;
                    }

                    // Hit shields
                    for (int s = 0; s < 4; s++) {
                        if (shieldHealth[s] > 0 && CheckCollisionRecs(enemyBullets[i].rect, shields[s])) {
                            enemyBullets[i].active = false;
                            shieldHealth[s]--;
                        }
                    }
                }
            }

            // Enemy movement
            bool edge = false;
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active) {
                    enemies[i].rect.x += enemySpeed * dir;
                    if (enemies[i].rect.x < 0 || enemies[i].rect.x + enemies[i].rect.width > SCREEN_WIDTH)
                        edge = true;
                }
            }
            if (edge) {
                dir *= -1;
                for (int i = 0; i < MAX_ENEMIES; i++)
                    if (enemies[i].active)
                        enemies[i].rect.y += 15;
            }

            // Bullet-Enemy collision
            for (int i = 0; i < 3; i++) {
                if (bullets[i].active) {
                    for (int e = 0; e < MAX_ENEMIES; e++) {
                        if (enemies[e].active && CheckCollisionRecs(bullets[i].rect, enemies[e].rect)) {
                            bullets[i].active = false;
                            enemies[e].health--;
                            if (enemies[e].health <= 0) {
                                enemies[e].active = false;
                                alive--;
                                score += (enemies[e].type + 1) * 10;
                                
                                // Spawn power-up randomly
                                if (!powerUpActive && rand() % 10 == 0) {
                                    powerUpActive = true;
                                    powerUp.x = enemies[e].rect.x;
                                    powerUp.y = enemies[e].rect.y;
                                    powerUpTimer = 0;
                                }
                            }
                            break;
                        }
                    }

                    // Bullet-Shield collision
                    for (int s = 0; s < 4; s++) {
                        if (shieldHealth[s] > 0 && CheckCollisionRecs(bullets[i].rect, shields[s])) {
                            bullets[i].active = false;
                            shieldHealth[s]--;
                        }
                    }
                }
            }

            // Power-up movement and collision
            if (powerUpActive) {
                powerUp.y += 2;
                powerUpTimer += delta;
                if (powerUp.y > SCREEN_HEIGHT || powerUpTimer > 8.0f) {
                    powerUpActive = false;
                }
                if (CheckCollisionRecs(powerUp, player)) {
                    powerUpActive = false;
                    rapidFire = true;
                    rapidFireTimer = 5.0f;
                }
            }

            // Win/Lose conditions
            if (alive == 0) {
                victory = true;
            }
            
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i].active && enemies[i].rect.y + enemies[i].rect.height > player.y) {
                    gameOver = true;
                }
            }

            // Next level
            if (victory && IsKeyPressed(KEY_ENTER)) {
                level++;
                alive = MAX_ENEMIES;
                victory = false;
                enemySpeed += 0.5f;
                
                // Reset enemies
                enemyCount = 0;
                for (int row = 0; row < 3; row++) {
                    for (int col = 0; col < 8; col++) {
                        enemies[enemyCount].rect = (Rectangle){50 + col*85, 60 + row*50, 30, 20};
                        enemies[enemyCount].active = true;
                        enemies[enemyCount].type = row;
                        enemies[enemyCount].health = (row == 2) ? 2 : 1;
                        enemyCount++;
                    }
                }
                
                // Reset shields
                for (int i = 0; i < 4; i++) shieldHealth[i] = 3;
            }
        }

        // Restart game
        if ((gameOver || victory) && IsKeyPressed(KEY_R)) {
            gameOver = false;
            victory = false;
            playerLives = 3;
            score = 0;
            level = 1;
            alive = MAX_ENEMIES;
            enemySpeed = 1.5f;
            dir = 1;
            rapidFire = false;
            invincible = false;
            
            // Reset everything
            for (int i = 0; i < 3; i++) bullets[i].active = false;
            for (int i = 0; i < MAX_ENEMY_BULLETS; i++) enemyBullets[i].active = false;
            powerUpActive = false;
            
            enemyCount = 0;
            for (int row = 0; row < 3; row++) {
                for (int col = 0; col < 8; col++) {
                    enemies[enemyCount].rect = (Rectangle){50 + col*85, 60 + row*50, 30, 20};
                    enemies[enemyCount].active = true;
                    enemies[enemyCount].type = row;
                    enemies[enemyCount].health = (row == 2) ? 2 : 1;
                    enemyCount++;
                }
            }
            
            for (int i = 0; i < 4; i++) shieldHealth[i] = 3;
        }

        // Drawing
        BeginDrawing();
        ClearBackground(BLACK);

        // Player (flash when invincible)
        if (!invincible || ((int)(invincibleTimer * 10) % 2 == 0))
            DrawRectangleRec(player, GREEN);

        // Player bullets
        for (int i = 0; i < 3; i++)
            if (bullets[i].active)
                DrawRectangleRec(bullets[i].rect, YELLOW);

        // Enemy bullets
        for (int i = 0; i < MAX_ENEMY_BULLETS; i++)
            if (enemyBullets[i].active)
                DrawRectangleRec(enemyBullets[i].rect, RED);

        // Enemies
        for (int i = 0; i < MAX_ENEMIES; i++) {
            if (enemies[i].active) {
                Color enemyColor = WHITE;
                if (enemies[i].type == 1) enemyColor = SKYBLUE;
                if (enemies[i].type == 2) enemyColor = ORANGE;
                DrawRectangleRec(enemies[i].rect, enemyColor);
                
                // Health indicator for tougher enemies
                if (enemies[i].health > 1) {
                    DrawRectangle(enemies[i].rect.x, enemies[i].rect.y - 5, 
                                enemies[i].rect.width * enemies[i].health / 2, 2, LIME);
                }
            }
        }

        // Shields
        for (int i = 0; i < 4; i++) {
            if (shieldHealth[i] > 0) {
                Color shieldColor = GREEN;
                if (shieldHealth[i] == 2) shieldColor = YELLOW;
                if (shieldHealth[i] == 1) shieldColor = ORANGE;
                DrawRectangleRec(shields[i], shieldColor);
            }
        }

        // Power-up
        if (powerUpActive) {
            DrawRectangle(powerUp.x, powerUp.y, powerUp.width, powerUp.height, GOLD);
            DrawText("P", powerUp.x + 6, powerUp.y + 3, 15, BLACK);
        }

        // HUD
        DrawText(TextFormat("SCORE: %d", score), 10, 10, 20, WHITE);
        DrawText(TextFormat("LEVEL: %d", level), 10, 35, 20, WHITE);
        DrawText(TextFormat("LIVES: %d", playerLives), SCREEN_WIDTH - 120, 10, 20, WHITE);
        
        if (rapidFire) {
            DrawText("RAPID FIRE!", SCREEN_WIDTH - 150, 35, 15, GOLD);
        }

        // Game over / Victory
        if (gameOver) {
            DrawText("GAME OVER!", 280, 250, 40, RED);
            DrawText("Press R to Restart", 270, 300, 25, WHITE);
        }

        if (victory) {
            DrawText("LEVEL COMPLETE!", 240, 250, 40, YELLOW);
            DrawText(TextFormat("Score: %d", score), 320, 300, 25, WHITE);
            DrawText("Press ENTER for Next Level", 210, 340, 20, WHITE);
            DrawText("Press R to Restart", 270, 370, 20, WHITE);
        }

        EndDrawing();
    }

    CloseAudioDevice();
    CloseWindow();
    return 0;
}
