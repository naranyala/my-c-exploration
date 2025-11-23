// easing_demo.c - Interactive raylib demo for easing functions
// Compile: gcc easing_demo.c -o easing_demo -lraylib -lm
// Or with raylib installed: cc easing_demo.c $(pkg-config --libs --cflags raylib) -lm

#include "raylib.h"
#include "../lib/easing.h"
#include <stdio.h>
#include <string.h>

#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800
#define MAX_EASINGS 24

typedef struct {
    const char* name;
    float (*func)(float);
    Color color;
} EasingType;

// Global easing functions array
EasingType easings[MAX_EASINGS] = {
    {"Linear", ease_linear, GRAY},
    {"In Quad", ease_in_quad, RED},
    {"Out Quad", ease_out_quad, MAROON},
    {"InOut Quad", ease_in_out_quad, PINK},
    {"In Cubic", ease_in_cubic, ORANGE},
    {"Out Cubic", ease_out_cubic, GOLD},
    {"InOut Cubic", ease_in_out_cubic, YELLOW},
    {"In Quart", ease_in_quart, LIME},
    {"Out Quart", ease_out_quart, GREEN},
    {"InOut Quart", ease_in_out_quart, DARKGREEN},
    {"In Sine", ease_in_sine, SKYBLUE},
    {"Out Sine", ease_out_sine, BLUE},
    {"InOut Sine", ease_in_out_sine, DARKBLUE},
    {"In Expo", ease_in_expo, PURPLE},
    {"Out Expo", ease_out_expo, VIOLET},
    {"InOut Expo", ease_in_out_expo, DARKPURPLE},
    {"In Elastic", ease_in_elastic, MAGENTA},
    {"Out Elastic", ease_out_elastic, PINK},
    {"InOut Elastic", ease_in_out_elastic, BEIGE},
    {"In Bounce", ease_in_bounce, BROWN},
    {"Out Bounce", ease_out_bounce, ORANGE},
    {"InOut Bounce", ease_in_out_bounce, DARKBROWN},
    {"In Back", ease_in_back, RAYWHITE},
    {"Out Back", ease_out_back, LIGHTGRAY}
};

void DrawGraph(int x, int y, int width, int height, int selectedEasing, bool showAll) {
    // Draw background
    DrawRectangle(x, y, width, height, (Color){30, 30, 40, 255});
    DrawRectangleLines(x, y, width, height, WHITE);
    
    // Draw grid
    for (int i = 0; i <= 10; i++) {
        int gridX = x + (width * i) / 10;
        int gridY = y + (height * i) / 10;
        DrawLine(gridX, y, gridX, y + height, (Color){50, 50, 60, 255});
        DrawLine(x, gridY, x + width, gridY, (Color){50, 50, 60, 255});
    }
    
    // Draw diagonal reference line
    DrawLine(x, y + height, x + width, y, (Color){80, 80, 90, 255});
    
    // Draw axes labels
    DrawText("Time", x + width/2 - 20, y + height + 5, 20, LIGHTGRAY);
    DrawTextEx(GetFontDefault(), "Value", (Vector2){x - 50, y + height/2}, 20, 2, LIGHTGRAY);
    
    // Draw curves
    if (showAll) {
        for (int e = 0; e < MAX_EASINGS; e++) {
            for (int i = 0; i < width - 1; i++) {
                float t1 = (float)i / width;
                float t2 = (float)(i + 1) / width;
                float v1 = easings[e].func(t1);
                float v2 = easings[e].func(t2);
                
                int x1 = x + i;
                int y1 = y + height - (int)(v1 * height);
                int x2 = x + i + 1;
                int y2 = y + height - (int)(v2 * height);
                
                Color col = easings[e].color;
                col.a = 150;
                DrawLine(x1, y1, x2, y2, col);
            }
        }
    } else {
        // Draw single curve with thicker line
        for (int i = 0; i < width - 1; i++) {
            float t1 = (float)i / width;
            float t2 = (float)(i + 1) / width;
            float v1 = easings[selectedEasing].func(t1);
            float v2 = easings[selectedEasing].func(t2);
            
            int x1 = x + i;
            int y1 = y + height - (int)(v1 * height);
            int x2 = x + i + 1;
            int y2 = y + height - (int)(v2 * height);
            
            DrawLineEx((Vector2){x1, y1}, (Vector2){x2, y2}, 3, easings[selectedEasing].color);
        }
    }
}

void DrawAnimatedBall(int x, int y, float t, int selectedEasing) {
    float eased = easings[selectedEasing].func(t);
    int ballX = x + (int)(eased * 300);
    DrawCircle(ballX, y, 20, easings[selectedEasing].color);
    DrawCircleLines(ballX, y, 20, WHITE);
    
    // Draw path
    DrawLine(x, y, x + 300, y, (Color){80, 80, 90, 255});
    DrawCircle(x, y, 5, GREEN);
    DrawCircle(x + 300, y, 5, RED);
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Easing Functions Demo");
    SetTargetFPS(60);
    
    int selectedEasing = 21; // Start with Out Bounce
    bool isAnimating = true;
    float animTime = 0.0f;
    float animSpeed = 1.0f;
    bool showAllCurves = false;
    
    while (!WindowShouldClose()) {
        // Update
        if (isAnimating) {
            animTime += GetFrameTime() * animSpeed;
            if (animTime > 1.0f) animTime = 0.0f;
        }
        
        // Input handling
        if (IsKeyPressed(KEY_SPACE)) isAnimating = !isAnimating;
        if (IsKeyPressed(KEY_R)) animTime = 0.0f;
        if (IsKeyPressed(KEY_UP)) selectedEasing = (selectedEasing + 1) % MAX_EASINGS;
        if (IsKeyPressed(KEY_DOWN)) selectedEasing = (selectedEasing - 1 + MAX_EASINGS) % MAX_EASINGS;
        if (IsKeyPressed(KEY_RIGHT)) animSpeed += 0.25f;
        if (IsKeyPressed(KEY_LEFT)) animSpeed = fmaxf(0.25f, animSpeed - 0.25f);
        if (IsKeyPressed(KEY_A)) showAllCurves = !showAllCurves;
        
        // Mouse wheel for easing selection
        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            selectedEasing = (selectedEasing - (int)wheel + MAX_EASINGS) % MAX_EASINGS;
        }
        
        // Draw
        BeginDrawing();
        ClearBackground((Color){20, 20, 30, 255});
        
        // Title
        DrawText("EASING FUNCTIONS VISUALIZER", 20, 20, 30, WHITE);
        DrawText(TextFormat("Current: %s", easings[selectedEasing].name), 20, 60, 24, easings[selectedEasing].color);
        
        // Graph
        DrawGraph(50, 120, 700, 500, selectedEasing, showAllCurves);
        
        // Animated ball
        DrawText("Animation:", 800, 120, 20, WHITE);
        DrawAnimatedBall(800, 180, animTime, selectedEasing);
        
        // Info panel
        int infoY = 250;
        DrawText("Controls:", 800, infoY, 20, YELLOW);
        DrawText("UP/DOWN: Change easing", 800, infoY + 30, 16, WHITE);
        DrawText("SPACE: Pause/Resume", 800, infoY + 55, 16, WHITE);
        DrawText("R: Reset animation", 800, infoY + 80, 16, WHITE);
        DrawText("LEFT/RIGHT: Speed", 800, infoY + 105, 16, WHITE);
        DrawText("A: Toggle all curves", 800, infoY + 130, 16, WHITE);
        DrawText("Mouse Wheel: Navigate", 800, infoY + 155, 16, WHITE);
        
        DrawText(TextFormat("Speed: %.2fx", animSpeed), 800, infoY + 190, 16, LIGHTGRAY);
        DrawText(TextFormat("Time: %.2f", animTime), 800, infoY + 215, 16, LIGHTGRAY);
        DrawText(TextFormat("Value: %.3f", easings[selectedEasing].func(animTime)), 800, infoY + 240, 16, LIGHTGRAY);
        
        // Easing list
        int listY = 550;
        DrawText("Available Easings:", 800, listY, 18, YELLOW);
        int visibleStart = (selectedEasing / 8) * 8;
        for (int i = visibleStart; i < visibleStart + 8 && i < MAX_EASINGS; i++) {
            Color textColor = (i == selectedEasing) ? easings[i].color : LIGHTGRAY;
            DrawText(TextFormat("%s%s", (i == selectedEasing) ? "> " : "  ", easings[i].name), 
                     800, listY + 30 + (i - visibleStart) * 20, 16, textColor);
        }
        
        // Progress bar
        DrawRectangle(50, 650, 700, 30, (Color){50, 50, 60, 255});
        DrawRectangle(50, 650, (int)(700 * animTime), 30, easings[selectedEasing].color);
        DrawRectangleLines(50, 650, 700, 30, WHITE);
        
        // Status
        DrawText(isAnimating ? "Playing" : "Paused", 50, 690, 20, isAnimating ? GREEN : RED);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
