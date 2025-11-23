#include "raylib.h"
#include <stdio.h>
#include <string.h>

#define MAX_INPUT_LENGTH 100
#define KEY_COUNT 12

// Event types
typedef enum {
    EVENT_NONE,
    EVENT_KEY_PRESS,
    EVENT_KEY_RELEASE,
    EVENT_KEY_HELD
} EventType;

// Key button structure
typedef struct {
    Rectangle bounds;
    char* label;
    char* letters;
    int keyIndex;
    bool isPressed;
    float pressTime;
} KeyButton;

// Event structure
typedef struct {
    EventType type;
    int keyIndex;
    float timestamp;
} KeyEvent;

// Global state
typedef struct {
    char inputText[MAX_INPUT_LENGTH];
    int inputLength;
    int lastKeyPressed;
    float lastKeyTime;
    int currentLetterIndex;
    KeyButton keys[KEY_COUNT];
    KeyEvent currentEvent;
} KeyboardState;

// Initialize keyboard buttons
void InitKeyboard(KeyboardState* state) {
    const char* labels[] = {"1", "2\nABC", "3\nDEF", "4\nGHI", "5\nJKL", 
                            "6\nMNO", "7\nPQRS", "8\nTUV", "9\nWXYZ", 
                            "*", "0\n ", "#"};
    const char* letters[] = {"1", "abc2", "def3", "ghi4", "jkl5", 
                             "mno6", "pqrs7", "tuv8", "wxyz9", 
                             "*", " 0", "#"};
    
    int buttonWidth = 100;
    int buttonHeight = 80;
    int padding = 10;
    int startX = 50;
    int startY = 200;
    
    for (int i = 0; i < KEY_COUNT; i++) {
        int row = i / 3;
        int col = i % 3;
        
        state->keys[i].bounds = (Rectangle){
            startX + col * (buttonWidth + padding),
            startY + row * (buttonHeight + padding),
            buttonWidth,
            buttonHeight
        };
        state->keys[i].label = (char*)labels[i];
        state->keys[i].letters = (char*)letters[i];
        state->keys[i].keyIndex = i;
        state->keys[i].isPressed = false;
        state->keys[i].pressTime = 0.0f;
    }
    
    state->inputText[0] = '\0';
    state->inputLength = 0;
    state->lastKeyPressed = -1;
    state->lastKeyTime = 0.0f;
    state->currentLetterIndex = 0;
    state->currentEvent.type = EVENT_NONE;
}

// Event handler for key press
void HandleKeyPress(KeyboardState* state, int keyIndex) {
    float currentTime = GetTime();
    float timeSinceLastPress = currentTime - state->lastKeyTime;
    
    // Multi-tap logic
    if (keyIndex == state->lastKeyPressed && timeSinceLastPress < 1.0f) {
        // Same key pressed within time window - cycle letters
        if (state->inputLength > 0) {
            state->inputLength--; // Remove last character
        }
        state->currentLetterIndex++;
        
        char* letters = state->keys[keyIndex].letters;
        int lettersCount = strlen(letters);
        state->currentLetterIndex %= lettersCount;
        
        char newChar = letters[state->currentLetterIndex];
        state->inputText[state->inputLength++] = newChar;
        state->inputText[state->inputLength] = '\0';
    } else {
        // Different key or timeout - add first letter
        state->currentLetterIndex = 0;
        char* letters = state->keys[keyIndex].letters;
        
        if (state->inputLength < MAX_INPUT_LENGTH - 1) {
            state->inputText[state->inputLength++] = letters[0];
            state->inputText[state->inputLength] = '\0';
        }
    }
    
    state->lastKeyPressed = keyIndex;
    state->lastKeyTime = currentTime;
    state->keys[keyIndex].isPressed = true;
    state->keys[keyIndex].pressTime = currentTime;
}

// Event handler for key release
void HandleKeyRelease(KeyboardState* state, int keyIndex) {
    state->keys[keyIndex].isPressed = false;
}

// Process events (event loop)
void ProcessEvents(KeyboardState* state) {
    Vector2 mousePos = GetMousePosition();
    
    // Check for mouse clicks on keys
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < KEY_COUNT; i++) {
            if (CheckCollisionPointRec(mousePos, state->keys[i].bounds)) {
                state->currentEvent.type = EVENT_KEY_PRESS;
                state->currentEvent.keyIndex = i;
                state->currentEvent.timestamp = GetTime();
                HandleKeyPress(state, i);
                break;
            }
        }
    }
    
    // Check for mouse release
    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < KEY_COUNT; i++) {
            if (state->keys[i].isPressed) {
                state->currentEvent.type = EVENT_KEY_RELEASE;
                state->currentEvent.keyIndex = i;
                state->currentEvent.timestamp = GetTime();
                HandleKeyRelease(state, i);
            }
        }
    }
    
    // Backspace functionality
    if (IsKeyPressed(KEY_BACKSPACE) && state->inputLength > 0) {
        state->inputLength--;
        state->inputText[state->inputLength] = '\0';
        state->lastKeyPressed = -1;
    }
    
    // Clear functionality
    if (IsKeyPressed(KEY_ESCAPE)) {
        state->inputText[0] = '\0';
        state->inputLength = 0;
        state->lastKeyPressed = -1;
    }
}

// Render the keyboard
void RenderKeyboard(KeyboardState* state) {
    // Draw input display
    DrawRectangle(50, 50, 330, 100, LIGHTGRAY);
    DrawRectangleLines(50, 50, 330, 100, DARKGRAY);
    DrawText(state->inputText, 60, 80, 30, BLACK);
    
    // Draw instructions
    DrawText("Click keys to type (multi-tap)", 50, 20, 16, DARKGRAY);
    DrawText("BACKSPACE to delete, ESC to clear", 50, 160, 14, GRAY);
    
    // Draw keys
    for (int i = 0; i < KEY_COUNT; i++) {
        Color btnColor = state->keys[i].isPressed ? DARKGRAY : GRAY;
        
        DrawRectangleRec(state->keys[i].bounds, btnColor);
        DrawRectangleLinesEx(state->keys[i].bounds, 2, BLACK);
        
        // Draw label (center aligned)
        Vector2 textSize = MeasureTextEx(GetFontDefault(), state->keys[i].label, 20, 1);
        Vector2 textPos = {
            state->keys[i].bounds.x + (state->keys[i].bounds.width - textSize.x) / 2,
            state->keys[i].bounds.y + (state->keys[i].bounds.height - textSize.y) / 2
        };
        
        DrawText(state->keys[i].label, textPos.x, textPos.y, 20, WHITE);
    }
    
    // Show current event info
    if (state->currentEvent.type != EVENT_NONE) {
        const char* eventName = state->currentEvent.type == EVENT_KEY_PRESS ? "PRESS" : "RELEASE";
        DrawText(TextFormat("Last Event: %s Key %d", eventName, state->currentEvent.keyIndex), 
                 50, 550, 16, BLUE);
    }
}

int main(void) {
    const int screenWidth = 430;
    const int screenHeight = 600;
    
    InitWindow(screenWidth, screenHeight, "Mobile Phone Keyboard - Event Driven");
    SetTargetFPS(60);
    
    KeyboardState state = {0};
    InitKeyboard(&state);
    
    // Main event loop
    while (!WindowShouldClose()) {
        // Process all events
        ProcessEvents(&state);
        
        // Render
        BeginDrawing();
        ClearBackground(RAYWHITE);
        RenderKeyboard(&state);
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
