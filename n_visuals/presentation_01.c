#include "raylib.h"
#include "../lib/gp2d.h"
#include <stdio.h>
#include <string.h>


#define SCREEN_WIDTH    1200
#define SCREEN_HEIGHT   700
#define MAX_SLIDES      10

typedef struct {
    const char *title;
    const char *body;
    Color title_color;
    Color body_color;
    void (*draw_canvas)(float progress);  // Optional custom drawing, progress 0..1
} Slide;

void draw_slide_canvas1(float t) {
    gp2d_vec2 center = gp2d_v2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    float radius = 80 + 120 * gp2d_ease_out_cubic(t);
    DrawRing((Vector2){center.x, center.y}, radius-20, radius, 0, 360, 40, Fade(BLUE, 0.7f));
    DrawCircle(center.x, center.y, 30 * t, WHITE);
}

void draw_slide_canvas2(float t) {
    int segments = 6;
    float radius = 150;
    gp2d_vec2 c = gp2d_v2(SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    
    for (int i = 0; i < segments; i++) {
        float angle = i * PI * 2 / segments + t * 2;
        gp2d_vec2 p = gp2d_v2_add(c, gp2d_v2_from_angle(angle));
        p = gp2d_v2_scale(p, radius);
        float size = 20 + 30 * sinf(t * 4 + i);
        DrawCircle(p.x, p.y, size, Fade(RAYWHITE, 0.8f));
    }
}

Slide slides[] = {
    { "Welcome to gp2d.h + raylib", 
      "A tiny 2D toolkit meets the best immediate-mode GUI library\n\n"
      "Press ← → or SPACE to navigate\nClick anywhere to advance", 
      WHITE, LIGHTGRAY, NULL },

    { "Smooth Transitions", 
      "All slides use cubic easing + vector interpolation\n"
      "Thanks to gp2d_ease_in_out_cubic() and gp2d_v2_lerp()", 
      YELLOW, WHITE, NULL },

    { "Custom Canvas Drawing", 
      "You can draw anything per-slide\n"
      "Look at the animated ring →", 
      SKYBLUE, WHITE, draw_slide_canvas1 },

    { "Math Made Easy", 
      "• gp2d_vec2 operations\n"
      "• Bezier curves\n"
      "• 3×3 transforms\n"
      "• Easing functions\n"
      "All header-only, zero dependencies", 
      GREEN, WHITE, NULL },

    { "Live Animation Demo", 
      "Rotating hexagon particles using:\n"
      "  gp2d_v2_from_angle()\n"
      "  gp2d_ease_out_cubic() timing", 
      PURPLE, WHITE, draw_slide_canvas2 },

    { "Thank You!", 
      "gp2d.h + raylib = perfect combo for demos, tools, games\n\n"
      "Happy coding! 🚀", 
      RAYWHITE, GOLD, NULL },
};

int current_slide = 0;
int total_slides = sizeof(slides)/sizeof(slides[0]);
float transition = 1.0f;        // 0.0 = fully on previous, 1.0 = fully on current
float transition_speed = 0.9f;  // lower = slower

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "gp2d.h + raylib Presentation Demo");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        // --- Input ---
        if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_SPACE) || IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) && transition > 0.99f) {
            if (current_slide < total_slides - 1) {
                current_slide++;
                transition = 0.0f;
            }
        }
        if (IsKeyPressed(KEY_LEFT) && transition > 0.99f) {
            if (current_slide > 0) {
                current_slide--;
                transition = 0.0f;
            }
        }

        // --- Update transition ---
        if (transition < 1.0f) {
            transition += transition_speed * GetFrameTime();
            if (transition > 1.0f) transition = 1.0f;
        }

        float t = gp2d_ease_in_out_cubic(transition);  // smooth ease

        BeginDrawing();
        ClearBackground(BLACK);

        // Background dim for style
        DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(BLACK, 0.6f));

        int prev = current_slide > 0 ? current_slide - 1 : current_slide;
        float offset_prev  = (1.0f - t) * -200;
        float offset_curr  = t * 0;

        // Draw previous slide (fading out)
        if (transition < 1.0f && current_slide > 0) {
            float alpha = 1.0f - t;
            DrawTextEx(GetFontDefault(), slides[prev].title,
                       (Vector2){ 100 + offset_prev, 120 }, 72, 4, Fade(slides[prev].title_color, alpha));
            DrawTextEx(GetFontDefault(), slides[prev].body,
                       (Vector2){ 100 + offset_prev, 260 }, 36, 2, Fade(slides[prev].body_color, alpha));
        }

        // Draw current slide
        {
            float alpha = 1.0f;
            DrawTextEx(GetFontDefault(), slides[current_slide].title,
                       (Vector2){ 100 + offset_curr, 120 }, 72, 4, Fade(slides[current_slide].title_color, alpha));
            DrawTextEx(GetFontDefault(), slides[current_slide].body,
                       (Vector2){ 100 + offset_curr, 260 }, 36, 2, Fade(slides[current_slide].body_color, alpha));

            // Custom canvas drawing if provided
            if (slides[current_slide].draw_canvas) {
                BeginScissorMode(650, 150, 500, 400);
                slides[current_slide].draw_canvas(t);
                EndScissorMode();
            }
        }

        // Navigation hint
        const char *hint = (current_slide < total_slides-1) ? "→ / SPACE / CLICK" : "END";
        DrawText(hint, SCREEN_WIDTH - MeasureText(hint, 30) - 30, SCREEN_HEIGHT - 60, 30, Fade(GRAY, 0.7f));
        DrawText(TextFormat("%d / %d", current_slide + 1, total_slides), 30, SCREEN_HEIGHT - 60, 30, GRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
