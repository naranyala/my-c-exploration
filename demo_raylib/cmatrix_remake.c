// Matrix Rain "Hacker" Demo for anim2d.h + Raylib
// Compile: gcc matrix.c -lraylib -lm -o matrix
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
#define MAX_STREAMS 60
#define MAX_GLYPHS 40

// Japanese katakana and random symbols
static const char* MATRIX_CHARS = "ｱｲｳｴｵｶｷｸｹｺｻｼｽｾｿﾀﾁﾂﾃﾄﾅﾆﾇﾈﾉﾊﾋﾌﾍﾎﾏﾐﾑﾒﾓﾔﾕﾖﾗﾘﾙﾚﾛﾜｦﾝ0123456789Z:.=*+-<>¦|ﾘｱ";
static int CHAR_COUNT = 0;

typedef struct {
    char symbol;
    float y;
    float opacity;
    float brightness;
} matrix_glyph;

typedef struct {
    anim_clip clip;
    matrix_glyph glyphs[MAX_GLYPHS];
    int glyph_count;
    float x;
    float speed;
    bool active;
    float change_timer;
    float change_interval;
    int head_idx;  // Which glyph is the bright "head"
} matrix_stream;

typedef struct {
    matrix_stream streams[MAX_STREAMS];
    int stream_count;
    float spawn_timer;
    float glitch_timer;
    bool glitch_active;
} matrix_field;

// Get random matrix character
char get_random_char(void) {
    return MATRIX_CHARS[GetRandomValue(0, CHAR_COUNT - 1)];
}

void spawn_stream(matrix_field* field) {
    if (field->stream_count >= MAX_STREAMS) return;
    
    matrix_stream* stream = &field->streams[field->stream_count++];
    stream->active = true;
    stream->x = GetRandomValue(0, SCREEN_W / 20) * 20;
    stream->speed = GetRandomValue(200, 500) / 100.0f;
    stream->change_timer = 0.0f;
    stream->change_interval = GetRandomValue(5, 15) / 100.0f;
    
    // Random length
    stream->glyph_count = GetRandomValue(8, MAX_GLYPHS);
    stream->head_idx = stream->glyph_count - 1;
    
    float duration = (SCREEN_H + stream->glyph_count * 20) / (stream->speed * 100);
    
    // Initialize glyphs
    for (int i = 0; i < stream->glyph_count; i++) {
        stream->glyphs[i].symbol = get_random_char();
        stream->glyphs[i].y = -i * 20;
        stream->glyphs[i].opacity = 1.0f - (i / (float)stream->glyph_count) * 0.7f;
        stream->glyphs[i].brightness = (i == stream->head_idx) ? 1.0f : 0.3f;
    }
    
    // Setup animation for the stream position
    anim_clip_reset(&stream->clip);
    anim_clip_config(&stream->clip, duration, 1.0f, 0.0f, ANIM_PLAY_ONCE);
    
    anim_track pos_track;
    anim_track_reset(&pos_track);
    anim_track_add_scalar(&pos_track, 0.0f, duration, -20, SCREEN_H + 100, anim_ease_linear);
    anim_clip_add(&stream->clip, pos_track);
}

void update_matrix(matrix_field* field, float dt) {
    // Spawn new streams
    field->spawn_timer += dt;
    if (field->spawn_timer >= 0.1f) {
        field->spawn_timer = 0.0f;
        if (GetRandomValue(0, 100) < 30) {  // 30% chance
            spawn_stream(field);
        }
    }
    
    // Random glitch effect
    field->glitch_timer += dt;
    if (field->glitch_timer >= 2.0f) {
        field->glitch_timer = 0.0f;
        field->glitch_active = GetRandomValue(0, 100) < 15;  // 15% chance
    }
    
    // Update streams
    for (int i = 0; i < field->stream_count; i++) {
        matrix_stream* stream = &field->streams[i];
        if (!stream->active) continue;
        
        int still_playing = anim_clip_update(&stream->clip, dt);
        
        // Get current offset
        anim_eval eval;
        anim_clip_eval(&stream->clip, &eval);
        
        if (eval.active) {
            float offset = eval.value.s;
            
            // Update glyph positions
            for (int j = 0; j < stream->glyph_count; j++) {
                stream->glyphs[j].y = offset - (j * 20);
            }
            
            // Randomly change symbols
            stream->change_timer += dt;
            if (stream->change_timer >= stream->change_interval) {
                stream->change_timer = 0.0f;
                int idx = GetRandomValue(0, stream->glyph_count - 1);
                stream->glyphs[idx].symbol = get_random_char();
            }
        }
        
        if (!still_playing) {
            stream->active = false;
        }
    }
    
    // Remove inactive streams
    int write_idx = 0;
    for (int read_idx = 0; read_idx < field->stream_count; read_idx++) {
        if (field->streams[read_idx].active) {
            if (write_idx != read_idx) {
                field->streams[write_idx] = field->streams[read_idx];
            }
            write_idx++;
        }
    }
    field->stream_count = write_idx;
}

void draw_matrix(const matrix_field* field) {
    for (int i = 0; i < field->stream_count; i++) {
        const matrix_stream* stream = &field->streams[i];
        if (!stream->active) continue;
        
        for (int j = 0; j < stream->glyph_count; j++) {
            const matrix_glyph* glyph = &stream->glyphs[j];
            
            // Skip if off screen
            if (glyph->y < -20 || glyph->y > SCREEN_H + 20) continue;
            
            // Color based on position in stream
            Color color;
            if (j == stream->head_idx) {
                // Bright white head
                color = (Color){255, 255, 255, (unsigned char)(glyph->opacity * 255)};
            } else {
                // Green tail with fade
                float brightness = glyph->brightness;
                if (field->glitch_active && GetRandomValue(0, 100) < 5) {
                    // Random glitch color
                    color = (Color){
                        GetRandomValue(0, 255),
                        GetRandomValue(200, 255),
                        GetRandomValue(0, 100),
                        (unsigned char)(glyph->opacity * 200)
                    };
                } else {
                    color = (Color){
                        (unsigned char)(brightness * 100),
                        (unsigned char)(brightness * 255),
                        (unsigned char)(brightness * 100),
                        (unsigned char)(glyph->opacity * 200)
                    };
                }
            }
            
            // Draw glow for head
            if (j == stream->head_idx) {
                DrawCircle(stream->x + 5, glyph->y + 10, 12, Fade(color, 0.3f));
            }
            
            // Draw character
            char text[2] = {glyph->symbol, '\0'};
            DrawText(text, stream->x, glyph->y, 20, color);
        }
    }
}

void draw_scanlines(float time) {
    for (int y = 0; y < SCREEN_H; y += 4) {
        float alpha = sinf(time * 2.0f + y * 0.1f) * 0.02f + 0.03f;
        DrawRectangle(0, y, SCREEN_W, 2, Fade(GREEN, alpha));
    }
}

void draw_terminal_prompt(float time) {
    const char* lines[] = {
        "> SYSTEM ACCESS GRANTED",
        "> DECRYPTING DATA STREAM...",
        "> NEURAL INTERFACE ACTIVE",
        "> MATRIX PROTOCOL ENABLED"
    };
    
    int line_idx = ((int)time / 3) % 4;
    float alpha = (sinf(time * 3.0f) * 0.3f + 0.7f);
    
    DrawText(lines[line_idx], 20, SCREEN_H - 50, 20, Fade(GREEN, alpha));
    
    // Blinking cursor
    if ((int)(time * 2) % 2 == 0) {
        int text_width = MeasureText(lines[line_idx], 20);
        DrawRectangle(25 + text_width, SCREEN_H - 48, 12, 20, GREEN);
    }
}

void draw_glitch_lines(const matrix_field* field) {
    if (!field->glitch_active) return;
    
    for (int i = 0; i < 5; i++) {
        int y = GetRandomValue(0, SCREEN_H);
        int height = GetRandomValue(2, 8);
        DrawRectangle(0, y, SCREEN_W, height, Fade(GREEN, 0.3f));
    }
}

int main(void) {
    InitWindow(SCREEN_W, SCREEN_H, "Matrix Rain - anim2d Demo");
    SetTargetFPS(60);
    
    // Count characters
    CHAR_COUNT = strlen(MATRIX_CHARS);
    
    matrix_field field = {0};
    field.spawn_timer = 0.0f;
    field.stream_count = 0;
    field.glitch_timer = 0.0f;
    field.glitch_active = false;
    
    // Spawn initial streams
    for (int i = 0; i < 15; i++) {
        spawn_stream(&field);
        // Randomize start positions
        field.streams[i].clip.time = GetRandomValue(0, 100) / 100.0f;
    }
    
    float time_elapsed = 0.0f;
    
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        time_elapsed += dt;
        
        update_matrix(&field, dt);
        
        // Manual spawn on SPACE
        if (IsKeyPressed(KEY_SPACE)) {
            for (int i = 0; i < 10; i++) {
                spawn_stream(&field);
            }
        }
        
        // Toggle glitch on G
        if (IsKeyPressed(KEY_G)) {
            field.glitch_active = !field.glitch_active;
            field.glitch_timer = 0.0f;
        }
        
        BeginDrawing();
        
        // Black background
        ClearBackground((Color){0, 0, 0, 255});
        
        // Subtle dark green tint overlay
        DrawRectangle(0, 0, SCREEN_W, SCREEN_H, Fade((Color){0, 20, 0, 255}, 0.3f));
        
        // Matrix rain
        draw_matrix(&field);
        
        // Scanline effect
        draw_scanlines(time_elapsed);
        
        // Glitch effect
        draw_glitch_lines(&field);
        
        // Terminal prompt
        draw_terminal_prompt(time_elapsed);
        
        // UI overlay
        DrawRectangle(0, 0, SCREEN_W, 80, Fade(BLACK, 0.7f));
        DrawText("THE MATRIX", 20, 15, 35, GREEN);
        DrawText("Press SPACE for more streams | Press G to toggle glitch", 20, 55, 16, DARKGREEN);
        DrawText(TextFormat("Active streams: %d", field.stream_count), SCREEN_W - 250, 20, 20, GREEN);
        
        // Vignette effect
        DrawRectangleGradientEx(
            (Rectangle){0, 0, SCREEN_W, 100},
            Fade(BLACK, 0.8f), Fade(BLACK, 0.8f), Fade(BLACK, 0.0f), Fade(BLACK, 0.0f)
        );
        DrawRectangleGradientEx(
            (Rectangle){0, SCREEN_H - 100, SCREEN_W, 100},
            Fade(BLACK, 0.0f), Fade(BLACK, 0.0f), Fade(BLACK, 0.8f), Fade(BLACK, 0.8f)
        );
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
