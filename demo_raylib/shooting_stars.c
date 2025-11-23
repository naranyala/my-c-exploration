// Shooting Stars Demo for anim2d.h + Raylib
// Compile: gcc shooting_stars.c -lraylib -lm -o stars
// Make sure anim2d.h is in the same directory

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#define ANIM2D_IMPLEMENTATION
#include "../lib/anim2d.h"
#include "raylib.h"

#define SCREEN_W 1400
#define SCREEN_H 900
#define MAX_STARS 50

typedef struct {
    anim_clip clip;
    bool active;
    float trail_length;
    float glow_size;
    Color color;
    Vector2 trail_points[20];
    int trail_count;
} shooting_star;

typedef struct {
    shooting_star stars[MAX_STARS];
    int star_count;
    float spawn_timer;
    float spawn_interval;
} star_field;

// Create a shooting star with random parameters
void spawn_star(star_field* field) {
    if (field->star_count >= MAX_STARS) return;
    
    shooting_star* star = &field->stars[field->star_count++];
    star->active = true;
    star->trail_count = 0;
    
    // Random spawn position at top
    float start_x = GetRandomValue(-200, SCREEN_W + 200);
    float start_y = GetRandomValue(-100, 100);
    
    // Random end position
    float end_x = start_x + GetRandomValue(-300, 300);
    float end_y = SCREEN_H + GetRandomValue(100, 300);
    
    // Random duration and trail length
    float duration = GetRandomValue(15, 35) / 10.0f;
    star->trail_length = GetRandomValue(80, 150);
    star->glow_size = GetRandomValue(8, 16);
    
    // Random color (white, yellow, blue tints)
    int color_type = GetRandomValue(0, 3);
    if (color_type == 0) {
        star->color = (Color){255, 255, 255, 255}; // White
    } else if (color_type == 1) {
        star->color = (Color){255, 255, 200, 255}; // Warm yellow
    } else if (color_type == 2) {
        star->color = (Color){200, 220, 255, 255}; // Cool blue
    } else {
        star->color = (Color){255, 230, 200, 255}; // Orange tint
    }
    
    // Setup animation
    anim_clip_reset(&star->clip);
    anim_clip_config(&star->clip, duration, 1.0f, 0.0f, ANIM_PLAY_ONCE);
    
    // Position track
    anim_track pos_track;
    anim_track_reset(&pos_track);
    anim_vec2 start_pos = {start_x, start_y};
    anim_vec2 end_pos = {end_x, end_y};
    anim_track_add_vec2(&pos_track, 0.0f, duration, start_pos, end_pos, anim_ease_inquad);
    anim_clip_add(&star->clip, pos_track);
    
    // Opacity track (fade out at the end)
    anim_track opacity_track;
    anim_track_reset(&opacity_track);
    anim_track_add_scalar(&opacity_track, 0.0f, duration * 0.7f, 1.0f, 1.0f, anim_ease_linear);
    anim_track_add_scalar(&opacity_track, duration * 0.7f, duration, 1.0f, 0.0f, anim_ease_outquad);
    anim_clip_add(&star->clip, opacity_track);
}

void update_stars(star_field* field, float dt) {
    // Update spawn timer
    field->spawn_timer += dt;
    if (field->spawn_timer >= field->spawn_interval) {
        field->spawn_timer = 0.0f;
        field->spawn_interval = GetRandomValue(5, 20) / 10.0f; // 0.5 to 2 seconds
        spawn_star(field);
    }
    
    // Update active stars
    for (int i = 0; i < field->star_count; i++) {
        shooting_star* star = &field->stars[i];
        if (!star->active) continue;
        
        int still_playing = anim_clip_update(&star->clip, dt);
        
        // Get current position for trail
        anim_eval evals[2];
        anim_clip_eval(&star->clip, evals);
        
        if (evals[0].active) {
            Vector2 pos = {evals[0].value.v.x, evals[0].value.v.y};
            
            // Add to trail (every few frames)
            if (star->trail_count < 20) {
                star->trail_points[star->trail_count++] = pos;
            } else {
                // Shift trail
                for (int j = 0; j < 19; j++) {
                    star->trail_points[j] = star->trail_points[j + 1];
                }
                star->trail_points[19] = pos;
            }
        }
        
        if (!still_playing) {
            star->active = false;
        }
    }
    
    // Remove inactive stars (compact array)
    int write_idx = 0;
    for (int read_idx = 0; read_idx < field->star_count; read_idx++) {
        if (field->stars[read_idx].active) {
            if (write_idx != read_idx) {
                field->stars[write_idx] = field->stars[read_idx];
            }
            write_idx++;
        }
    }
    field->star_count = write_idx;
}

void draw_stars(const star_field* field) {
    for (int i = 0; i < field->star_count; i++) {
        const shooting_star* star = &field->stars[i];
        if (!star->active) continue;
        
        anim_eval evals[2];
        anim_clip_eval(&star->clip, evals);
        
        if (evals[0].active && evals[1].active) {
            Vector2 pos = {evals[0].value.v.x, evals[0].value.v.y};
            float opacity = evals[1].value.s;
            
            // Draw trail with gradient fade
            for (int j = 0; j < star->trail_count - 1; j++) {
                float trail_alpha = (float)j / (float)star->trail_count;
                trail_alpha *= opacity;
                
                Color trail_color = star->color;
                trail_color.a = (unsigned char)(trail_alpha * 100);
                
                float thickness = (trail_alpha * 3.0f) + 1.0f;
                DrawLineEx(star->trail_points[j], star->trail_points[j + 1], thickness, trail_color);
            }
            
            // Draw glow layers
            for (int g = 3; g >= 0; g--) {
                float glow_radius = star->glow_size + (g * 4);
                float glow_alpha = (1.0f - (g * 0.25f)) * opacity * 0.4f;
                Color glow = star->color;
                glow.a = (unsigned char)(glow_alpha * 255);
                DrawCircleV(pos, glow_radius, glow);
            }
            
            // Draw bright core
            Color core = star->color;
            core.a = (unsigned char)(opacity * 255);
            DrawCircleV(pos, star->glow_size * 0.5f, core);
            
            // Draw sparkle effect (cross)
            float sparkle_len = star->glow_size * 1.5f;
            Color sparkle = WHITE;
            sparkle.a = (unsigned char)(opacity * 200);
            DrawLineEx(
                (Vector2){pos.x - sparkle_len, pos.y},
                (Vector2){pos.x + sparkle_len, pos.y},
                2.0f, sparkle
            );
            DrawLineEx(
                (Vector2){pos.x, pos.y - sparkle_len},
                (Vector2){pos.x, pos.y + sparkle_len},
                2.0f, sparkle
            );
        }
    }
}

// Draw twinkling background stars
void draw_background_stars(float time) {
    for (int i = 0; i < 150; i++) {
        float x = (i * 127 % SCREEN_W);
        float y = (i * 197 % SCREEN_H);
        float phase = i * 0.1f;
        float twinkle = sinf(time * 2.0f + phase) * 0.5f + 0.5f;
        
        Color star_color = WHITE;
        star_color.a = (unsigned char)(twinkle * 150 + 50);
        
        DrawCircleV((Vector2){x, y}, 1.5f, star_color);
    }
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Shooting Stars - anim2d Demo");
    SetTargetFPS(60);
    
    star_field field = {0};
    field.spawn_timer = 0.0f;
    field.spawn_interval = 0.5f;
    field.star_count = 0;
    
    // Spawn a few initial stars
    for (int i = 0; i < 5; i++) {
        spawn_star(&field);
        // Offset their start times
        field.stars[i].clip.time = GetRandomValue(0, 100) / 100.0f;
    }
    
    float time_elapsed = 0.0f;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        time_elapsed += dt;
        
        update_stars(&field, dt);
        
        // Manual spawn on SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            for (int i = 0; i < 5; i++) {
                spawn_star(&field);
            }
        }
        
        BeginDrawing();
        
        // Night sky gradient
        DrawRectangleGradientV(0, 0, SCREEN_W, SCREEN_H,
            (Color){5, 10, 30, 255},
            (Color){20, 20, 50, 255}
        );
        
        // Background stars
        draw_background_stars(time_elapsed);
        
        // Shooting stars
        draw_stars(&field);
        
        // UI
        DrawText("SHOOTING STARS", 20, 20, 40, WHITE);
        DrawText("Press SPACE to spawn more stars", 20, 70, 20, LIGHTGRAY);
        DrawText(TextFormat("Active stars: %d", field.star_count), 20, 100, 20, LIGHTGRAY);
        DrawText("Press ESC to exit", 20, 130, 20, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
