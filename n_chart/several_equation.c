// main.c - Example usage of canvasplotlib.h
#include "raylib.h"
#include "../lib/canvasplotlib.h"
#include <math.h>

// Parametric function definitions
float fx_circle(float t)    { return 5 * cosf(t); }
float fy_circle(float t)    { return 5 * sinf(t); }

float fx_spiral(float t)    { return t * cosf(t*3); }
float fy_spiral(float t)    { return t * sinf(t*3); }

float fx_heart(float t)     { return 2 * powf(sinf(t), 3); }
float fy_heart(float t)     { return 1.8 * (cosf(t) - 0.6*cosf(2*t) - 0.3*cosf(3*t) - 0.1*cosf(4*t)); }

float fx_rose(float t)      { return 5 * cosf(5*t/2) * cosf(t); }
float fy_rose(float t)      { return 5 * cosf(5*t/2) * sinf(t); }

float fx_lissajous(float t) { return 6 * sinf(3*t + 0.2); }
float fy_lissajous(float t) { return 5 * sinf(4*t); }

float fx_hypocycloid(float t) { float a=5, b=2; return (a-b)*cosf(t) + b*cosf((a-b)/b * t); }
float fy_hypocycloid(float t) { float a=5, b=2; return (a-b)*sinf(t) - b*sinf((a-b)/b * t); }

// Demo structure
typedef struct {
    const char* name;
    float (*fx)(float);
    float (*fy)(float);
    Color color;
    float t_min, t_max;
    int steps;
    bool animate;
} Demo;

Demo demos[] = {
    {"Circle",                fx_circle,      fy_circle,      SKYBLUE, 0, 2*PI, 500,  false},
    {"Logarithmic Spiral",    fx_spiral,      fy_spiral,      ORANGE,  0, 4,    2000, true},
    {"Heart Curve",           fx_heart,       fy_heart,       RED,     0, 2*PI, 800,  false},
    {"5-Petal Rose",          fx_rose,        fy_rose,        PINK,    0, 4*PI, 1000, false},
    {"Lissajous (3:4)",       fx_lissajous,   fy_lissajous,   PURPLE,  0, 2*PI, 1000, true},
    {"Hypocycloid (Astroid)", fx_hypocycloid, fy_hypocycloid, LIME,    0, 4*PI, 800,  false},
};

const int demo_count = sizeof(demos)/sizeof(demos[0]);

int main() {
    const int WIDTH = 1100;
    const int HEIGHT = 700;
    InitWindow(WIDTH, HEIGHT, "CanvasPlotLib - 2D Math Gallery");
    SetTargetFPS(60);

    PlotContext plot;
    PlotInit(&plot, WIDTH, HEIGHT);

    int current_demo = 0;
    float time = 0.0f;

    while (!WindowShouldClose()) {
        // Keyboard controls
        if (IsKeyPressed(KEY_RIGHT)) {
            current_demo = (current_demo + 1) % demo_count;
            time = 0.0f;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            current_demo = (current_demo - 1 + demo_count) % demo_count;
            time = 0.0f;
        }

        // Animate time for moving curves
        if (demos[current_demo].animate) {
            time += 0.03f;
        }

        BeginDrawing();
            PlotDrawGridAndAxes(&plot);
            DrawTitle(demos[current_demo].name);
            
            // Plot current demo
            PlotFunction(&plot, 
                        demos[current_demo].color,
                        demos[current_demo].t_min,
                        demos[current_demo].t_max,
                        demos[current_demo].steps,
                        demos[current_demo].fx,
                        demos[current_demo].fy,
                        time);
            
            // Instructions
            DrawText("Use LEFT/RIGHT arrow keys to switch demos", 20, HEIGHT - 30, 20, DARKGRAY);
            DrawText(TextFormat("Demo %d/%d", current_demo + 1, demo_count), WIDTH - 150, HEIGHT - 30, 20, DARKGRAY);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
