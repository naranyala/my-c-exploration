#include "lib/reactive.h"
#include "raylib.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

// --- Clock State ---

typedef struct {
  int hour;
  int minute;
  int second;
  char timeStr[9]; // HH:MM:SS\0
} ClockState;

// --- Actions ---

void Action_UpdateClock(ClockState *s) {
  time_t now;
  time(&now);
  struct tm *local = localtime(&now);

  if (s->second != local->tm_sec || s->minute != local->tm_min ||
      s->hour != local->tm_hour) {
    s->hour = local->tm_hour;
    s->minute = local->tm_min;
    s->second = local->tm_sec;

    snprintf(s->timeStr, 9, "%02d:%02d:%02d", s->hour, s->minute, s->second);

    // We only need to notify if something actually changed,
    // but here we check inside the action.
    // In a pure system, the action might return a new state or a diff.
  }
}

// --- View ---

void DrawClock(void *data, void *ctx) {
  ClockState *s = (ClockState *)data;
  (void)ctx;

  BeginDrawing();
  ClearBackground(RAYWHITE);

  int screenWidth = GetScreenWidth();
  int screenHeight = GetScreenHeight();

  // Draw Time
  int fontSize = 80;
  int textWidth = MeasureText(s->timeStr, fontSize);
  int x = (screenWidth - textWidth) / 2;
  int y = (screenHeight - fontSize) / 2;

  DrawText(s->timeStr, x, y, fontSize, DARKGRAY);

  // Draw Date (optional, just to show more info)
  DrawText("Reactive Clock", 20, 20, 20, LIGHTGRAY);

  EndDrawing();
}

int main(void) {
  const int screenWidth = 400;
  const int screenHeight = 200;

  InitWindow(screenWidth, screenHeight, "Reactive Clock");
  SetTargetFPS(60);

  // Initialize State
  ClockState clockState = {0};
  ReactiveState appState;
  InitReactiveState(&appState, &clockState);

  // Initial update
  Action_UpdateClock(&clockState);

  while (!WindowShouldClose()) {
    // Update State
    // We check time every frame, but only trigger updates if seconds change
    // Ideally we'd use a timer, but polling is fine for this demo.

    ClockState oldState = clockState;
    Action_UpdateClock(&clockState);

    // Simple diff check to avoid unnecessary notifications/redraws if we were
    // fully event driven But since Raylib is immediate mode, we draw every
    // frame anyway. However, to demonstrate the pattern, we could say:
    if (memcmp(&oldState, &clockState, sizeof(ClockState)) != 0) {
      SetState(&appState);
    }

    // Render
    DrawClock(appState.data, NULL);
  }

  CleanupReactiveState(&appState);
  CloseWindow();
  return 0;
}
