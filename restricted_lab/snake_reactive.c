#include "lib/reactive.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define GRID_SIZE 20
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define COLS (SCREEN_WIDTH / GRID_SIZE)
#define ROWS (SCREEN_HEIGHT / GRID_SIZE)
#define MAX_SNAKE_LENGTH 400

// --- Game State ---

typedef struct {
  Vector2 snake[MAX_SNAKE_LENGTH];
  int snakeLength;
  Vector2 food;
  Vector2 direction;
  bool gameOver;
  int score;
} GameState;

// --- Actions ---

void Action_SpawnFood(GameState *s) {
  // Simple random spawn, could overlap snake but fine for demo
  s->food.x = GetRandomValue(0, COLS - 1);
  s->food.y = GetRandomValue(0, ROWS - 1);
}

void Action_InitGame(GameState *s) {
  s->snakeLength = 3;
  s->snake[0] = (Vector2){COLS / 2, ROWS / 2};
  s->snake[1] = (Vector2){COLS / 2, ROWS / 2 + 1};
  s->snake[2] = (Vector2){COLS / 2, ROWS / 2 + 2};
  s->direction = (Vector2){0, -1}; // Moving Up
  s->gameOver = false;
  s->score = 0;
  Action_SpawnFood(s);
}

void Action_ChangeDirection(GameState *s, Vector2 newDir) {
  // Prevent 180 degree turns
  if (s->direction.x + newDir.x != 0 || s->direction.y + newDir.y != 0) {
    s->direction = newDir;
  }
}

void Action_Tick(GameState *s) {
  if (s->gameOver)
    return;

  // Move body
  for (int i = s->snakeLength - 1; i > 0; i--) {
    s->snake[i] = s->snake[i - 1];
  }

  // Move head
  s->snake[0].x += s->direction.x;
  s->snake[0].y += s->direction.y;

  // Wall Collision
  if (s->snake[0].x < 0 || s->snake[0].x >= COLS || s->snake[0].y < 0 ||
      s->snake[0].y >= ROWS) {
    s->gameOver = true;
    return;
  }

  // Self Collision
  for (int i = 1; i < s->snakeLength; i++) {
    if (s->snake[0].x == s->snake[i].x && s->snake[0].y == s->snake[i].y) {
      s->gameOver = true;
      return;
    }
  }

  // Food Collision
  if (s->snake[0].x == s->food.x && s->snake[0].y == s->food.y) {
    s->score += 10;
    if (s->snakeLength < MAX_SNAKE_LENGTH) {
      s->snakeLength++;
      // New segment will be at the position of the last segment (will uncurl
      // next tick)
      s->snake[s->snakeLength - 1] = s->snake[s->snakeLength - 2];
    }
    Action_SpawnFood(s);
  }
}

// --- View / Observers ---

void DrawGame(void *data, void *ctx) {
  GameState *s = (GameState *)data;
  (void)ctx;

  BeginDrawing();
  ClearBackground(RAYWHITE);

  if (s->gameOver) {
    DrawText("GAME OVER", SCREEN_WIDTH / 2 - 100, SCREEN_HEIGHT / 2 - 40, 40,
             RED);
    char scoreText[50];
    snprintf(scoreText, 50, "Final Score: %d", s->score);
    DrawText(scoreText, SCREEN_WIDTH / 2 - 80, SCREEN_HEIGHT / 2 + 10, 20,
             DARKGRAY);
    DrawText("Press R to Restart", SCREEN_WIDTH / 2 - 90,
             SCREEN_HEIGHT / 2 + 40, 20, GRAY);
  } else {
    // Draw Food
    DrawRectangle(s->food.x * GRID_SIZE, s->food.y * GRID_SIZE, GRID_SIZE,
                  GRID_SIZE, RED);

    // Draw Snake
    for (int i = 0; i < s->snakeLength; i++) {
      Color color = (i == 0) ? DARKGREEN : GREEN;
      DrawRectangle(s->snake[i].x * GRID_SIZE, s->snake[i].y * GRID_SIZE,
                    GRID_SIZE, GRID_SIZE, color);
      DrawRectangleLines(s->snake[i].x * GRID_SIZE, s->snake[i].y * GRID_SIZE,
                         GRID_SIZE, GRID_SIZE, DARKGREEN);
    }

    // Draw Score
    char scoreText[50];
    snprintf(scoreText, 50, "Score: %d", s->score);
    DrawText(scoreText, 10, 10, 20, DARKGRAY);
  }

  EndDrawing();
}

int main(void) {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Reactive Snake");
  SetTargetFPS(60);

  GameState gameState;
  Action_InitGame(&gameState);

  ReactiveState appState;
  InitReactiveState(&appState, &gameState);

  // Subscribe the renderer
  Subscribe(&appState, DrawGame, NULL);

  // Game Loop variables
  double lastMoveTime = 0;
  const double moveInterval = 0.1; // 100ms per tick

  while (!WindowShouldClose()) {
    // Input Handling
    if (IsKeyPressed(KEY_RIGHT))
      Action_ChangeDirection(&gameState, (Vector2){1, 0});
    if (IsKeyPressed(KEY_LEFT))
      Action_ChangeDirection(&gameState, (Vector2){-1, 0});
    if (IsKeyPressed(KEY_UP))
      Action_ChangeDirection(&gameState, (Vector2){0, -1});
    if (IsKeyPressed(KEY_DOWN))
      Action_ChangeDirection(&gameState, (Vector2){0, 1});

    if (gameState.gameOver && IsKeyPressed(KEY_R)) {
      Action_InitGame(&gameState);
      SetState(&appState); // Force redraw
    }

    // Game Tick
    double currentTime = GetTime();
    if (currentTime - lastMoveTime >= moveInterval) {
      Action_Tick(&gameState);
      SetState(&appState); // Notify observers (Renderer)
      lastMoveTime = currentTime;
    }

    // Note: Because Raylib clears buffer every frame, we actually need to draw
    // every frame, not just when state changes. The Reactive pattern is
    // slightly mismatched with immediate mode rendering unless we separate
    // "Update" and "Render" loops completely. But for this demo, we can just
    // manually call the observer or let the loop handle it.
    //
    // To be strictly "Reactive", the DrawGame should ONLY be called by Notify.
    // But Raylib window will flicker if we don't draw every frame.
    // So we will cheat slightly and call DrawGame directly in the loop if no
    // update happened, OR we can just rely on the SetState to draw when things
    // change, but that won't work for 60FPS animations if we had them.
    //
    // For a grid game like Snake, we only need to redraw when it moves.
    // However, Raylib requires BeginDrawing/EndDrawing every frame to keep the
    // window responsive. So we will just call DrawGame(appState.data, NULL)
    // every frame here, but we KEEP the Subscribe call to demonstrate that
    // other observers (like a sound engine) could react to the state change.

    DrawGame(appState.data, NULL);
  }

  CleanupReactiveState(&appState);
  CloseWindow();
  return 0;
}
