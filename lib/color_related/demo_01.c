// [file name]: demo_01.c
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define COLORX_IMPLEMENTATION
#include "colorx.h"

#define PALETTEX_IMPLEMENTATION
#include "palettex.h"

#define MAX_COLORS 20
#define SCREEN_WIDTH 1200
#define SCREEN_HEIGHT 800

typedef enum {
    DEMO_COMPLEMENTARY,
    DEMO_ANALOGOUS,
    DEMO_TRIADIC,
    DEMO_TETRADIC,
    DEMO_SQUARE,
    DEMO_MONOCHROME,
    DEMO_EVEN_SPACED,
    DEMO_RANDOM_GOLDEN,
    DEMO_COUNT
} DemoMode;

const char* demo_names[] = {
    "Complementary",
    "Analogous", 
    "Triadic",
    "Tetradic",
    "Square",
    "Monochrome",
    "Evenly Spaced",
    "Random Golden"
};

typedef struct {
    DemoMode mode;
    ColorX colors[MAX_COLORS];
    int color_count;
    float base_hue;
    float saturation;
    float value;
    float time;
    bool auto_rotate;
} ColorDemo;

// Generate colors based on current demo mode
void generate_colors(ColorDemo* demo) {
    switch(demo->mode) {
        case DEMO_COMPLEMENTARY:
            demo->color_count = 2;
            plt_complementary(demo->colors, demo->base_hue, demo->saturation, demo->value);
            break;
            
        case DEMO_ANALOGOUS:
            demo->color_count = 3;
            plt_analogous(demo->colors, demo->base_hue, demo->saturation, demo->value, 0.08f);
            break;
            
        case DEMO_TRIADIC:
            demo->color_count = 3;
            plt_triadic(demo->colors, demo->base_hue, demo->saturation, demo->value);
            break;
            
        case DEMO_TETRADIC:
            demo->color_count = 4;
            plt_tetradic(demo->colors, demo->base_hue, demo->saturation, demo->value);
            break;
            
        case DEMO_SQUARE:
            demo->color_count = 4;
            plt_square(demo->colors, demo->base_hue, demo->saturation, demo->value);
            break;
            
        case DEMO_MONOCHROME:
            demo->color_count = 5;
            plt_monochrome(demo->colors, demo->base_hue, demo->saturation, demo->value);
            break;
            
        case DEMO_EVEN_SPACED:
            demo->color_count = 8;
            plt_even(demo->colors, demo->color_count, demo->saturation, demo->value);
            break;
            
        case DEMO_RANDOM_GOLDEN:
            demo->color_count = 12;
            plt_random_golden(demo->colors, demo->color_count, demo->saturation, demo->value);
            break;
            
        default:
            // Handle DEMO_COUNT case
            demo->color_count = 1;
            demo->colors[0] = cx_from_hsv(0, 0, 0, 1.0f);
            break;
    }
}

// Convert ColorX to Raylib Color
Color to_raylib_color(ColorX cx) {
    return (Color){
        (unsigned char)(cx.r * 255),
        (unsigned char)(cx.g * 255), 
        (unsigned char)(cx.b * 255),
        (unsigned char)(cx.a * 255)
    };
}

// Draw a color swatch with information
void draw_color_swatch(ColorX color, int x, int y, int width, int height, int index) {
    Color rl_color = to_raylib_color(color);
    
    // Draw the color rectangle
    DrawRectangle(x, y, width, height, rl_color);
    
    // Draw border
    DrawRectangleLines(x, y, width, height, BLACK);
    
    // Draw color information
    char info[64];
    float h, s, v;
    cx_to_hsv(color, &h, &s, &v);
    
    // Choose text color based on brightness for better readability
    float brightness = (color.r * 0.299f + color.g * 0.587f + color.b * 0.114f);
    Color text_color = brightness > 0.6f ? BLACK : WHITE;
    
    snprintf(info, sizeof(info), "#%02X%02X%02X", 
             (int)(color.r * 255), (int)(color.g * 255), (int)(color.b * 255));
    DrawText(info, x + 5, y + height - 40, 12, text_color);
    
    snprintf(info, sizeof(info), "HSV: %.0f°,%.0f%%,%.0f%%", 
             h * 360, s * 100, v * 100);
    DrawText(info, x + 5, y + height - 25, 12, text_color);
    
    snprintf(info, sizeof(info), "Color %d", index + 1);
    DrawText(info, x + 5, y + 10, 14, text_color);
}

// Draw interactive controls
void draw_controls(ColorDemo* demo) {
    int panel_width = 300;
    int panel_x = SCREEN_WIDTH - panel_width;
    
    // Control panel background
    DrawRectangle(panel_x, 0, panel_width, SCREEN_HEIGHT, Fade(LIGHTGRAY, 0.9f));
    DrawRectangleLines(panel_x, 0, panel_width, SCREEN_HEIGHT, DARKGRAY);
    
    // Title
    DrawText("Color Palette Demo", panel_x + 10, 20, 24, DARKBLUE);
    DrawText("Using colorx.h + palettex.h", panel_x + 10, 50, 16, DARKGRAY);
    
    // Current mode
    DrawText(TextFormat("Mode: %s", demo_names[demo->mode]), panel_x + 10, 90, 20, MAROON);
    
    // Controls instructions
    DrawText("CONTROLS:", panel_x + 10, 130, 16, BLACK);
    DrawText("SPACE - Change palette mode", panel_x + 20, 155, 14, DARKGRAY);
    DrawText("A - Toggle auto-rotate hue", panel_x + 20, 175, 14, DARKGRAY);
    DrawText("R - Randomize parameters", panel_x + 20, 195, 14, DARKGRAY);
    DrawText("H/S/V - Adjust hue/sat/val", panel_x + 20, 215, 14, DARKGRAY);
    
    // Parameters
    DrawText("PARAMETERS:", panel_x + 10, 250, 16, BLACK);
    DrawText(TextFormat("Hue: %.2f", demo->base_hue), panel_x + 20, 275, 14, DARKBLUE);
    DrawText(TextFormat("Saturation: %.2f", demo->saturation), panel_x + 20, 295, 14, DARKGREEN);
    DrawText(TextFormat("Value: %.2f", demo->value), panel_x + 20, 315, 14, DARKPURPLE);
    DrawText(TextFormat("Colors: %d", demo->color_count), panel_x + 20, 335, 14, DARKBROWN);
    DrawText(TextFormat("Auto-rotate: %s", demo->auto_rotate ? "ON" : "OFF"), 
             panel_x + 20, 355, 14, demo->auto_rotate ? GREEN : RED);
    
    // Color theory info
    DrawText("COLOR THEORY:", panel_x + 10, 400, 16, BLACK);
    
    const char* theory_text = "";
    switch(demo->mode) {
        case DEMO_COMPLEMENTARY:
            theory_text = "Complementary colors are opposite\non the color wheel. They create\nhigh contrast and vibrancy.";
            break;
        case DEMO_ANALOGOUS:
            theory_text = "Analogous colors are adjacent on\nthe color wheel. They create\nharmonious and serene designs.";
            break;
        case DEMO_TRIADIC:
            theory_text = "Triadic colors are evenly spaced\nat 120° intervals. They offer\nstrong visual contrast while\nmaintaining balance.";
            break;
        case DEMO_TETRADIC:
            theory_text = "Tetradic (rectangle) uses four\ncolors forming a rectangle on\nthe color wheel. Complex but\nversatile.";
            break;
        case DEMO_SQUARE:
            theory_text = "Square colors are evenly spaced\nat 90° intervals. Creates dynamic\nand vibrant color schemes.";
            break;
        case DEMO_MONOCHROME:
            theory_text = "Monochrome uses variations in\nlightness and saturation of a\nsingle hue. Creates elegant and\nunified designs.";
            break;
        case DEMO_EVEN_SPACED:
            theory_text = "Evenly spaced colors around the\ncolor wheel. Great for categorical\ndata where all colors should\nhave equal importance.";
            break;
        case DEMO_RANDOM_GOLDEN:
            theory_text = "Uses the golden ratio to distribute\ncolors. Creates aesthetically\npleasing random distributions.";
            break;
        default:
            theory_text = "Select a color scheme to see\ntheory information here.";
            break;
    }
    
    DrawText(theory_text, panel_x + 20, 425, 14, DARKGRAY);
    
    // Library features showcase
    DrawText("LIBRARY FEATURES:", panel_x + 10, 550, 16, BLACK);
    DrawText("✓ HSV color conversion", panel_x + 20, 575, 14, DARKGREEN);
    DrawText("✓ Color palette generation", panel_x + 20, 595, 14, DARKGREEN);
    DrawText("✓ Color arithmetic ops", panel_x + 20, 615, 14, DARKGREEN);
    DrawText("✓ Multiple color schemes", panel_x + 20, 635, 14, DARKGREEN);
}

// Draw creative visualizations using the colors
void draw_color_visualization(ColorDemo* demo, int x, int y, int width, int height) {
    float time = demo->time;
    
    switch(demo->mode) {
        case DEMO_COMPLEMENTARY:
        case DEMO_ANALOGOUS:
        case DEMO_TRIADIC:
        case DEMO_TETRADIC:
        case DEMO_SQUARE: {
            // Draw rotating color wheel
            int center_x = x + width/2;
            int center_y = y + height/2;
            int radius = fminf(width, height) * 0.4f;
            
            for(int i = 0; i < demo->color_count; i++) {
                float angle = (2 * PI * i / demo->color_count) + time * 0.5f;
                float start_angle = angle - (PI / demo->color_count);
                float end_angle = angle + (PI / demo->color_count);
                
                Color sector_color = to_raylib_color(demo->colors[i]);
                DrawCircleSector((Vector2){center_x, center_y}, radius, 
                               start_angle * 180/PI, end_angle * 180/PI, 
                               36, sector_color);
            }
            break;
        }
        
        case DEMO_MONOCHROME: {
            // Draw gradient bars
            int bar_width = width / demo->color_count;
            for(int i = 0; i < demo->color_count; i++) {
                int bar_x = x + i * bar_width;
                int bar_height = height * (0.3f + 0.7f * (i / (float)(demo->color_count - 1)));
                DrawRectangle(bar_x, y + height - bar_height, bar_width, bar_height, 
                            to_raylib_color(demo->colors[i]));
            }
            break;
        }
        
        case DEMO_EVEN_SPACED:
        case DEMO_RANDOM_GOLDEN: {
            // Draw animated particles with colors
            for(int i = 0; i < demo->color_count * 5; i++) {
                int color_idx = i % demo->color_count;
                float particle_time = time + i * 0.2f;
                float px = x + width * 0.5f + cosf(particle_time + i) * width * 0.4f;
                float py = y + height * 0.5f + sinf(particle_time * 1.3f + i) * height * 0.4f;
                float size = 10 + 15 * sinf(particle_time * 2 + i);
                
                DrawCircle(px, py, size, to_raylib_color(demo->colors[color_idx]));
            }
            break;
        }
        
        default:
            // Default visualization for any unhandled modes
            DrawRectangle(x, y, width, height, GRAY);
            DrawText("Visualization not implemented", x + 10, y + 10, 20, WHITE);
            break;
    }
}

int main(void) {
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raylib Color Palette Demo - colorx.h + palettex.h");
    SetTargetFPS(60);
    
    srand(time(NULL));
    
    ColorDemo demo = {
        .mode = DEMO_COMPLEMENTARY,
        .base_hue = 0.3f,
        .saturation = 0.8f,
        .value = 0.9f,
        .auto_rotate = true
    };
    
    generate_colors(&demo);
    
    while (!WindowShouldClose()) {
        float delta_time = GetFrameTime();
        demo.time += delta_time;
        
        // Handle input
        if (IsKeyPressed(KEY_SPACE)) {
            demo.mode = (demo.mode + 1) % DEMO_COUNT;
            generate_colors(&demo);
        }
        
        if (IsKeyPressed(KEY_A)) {
            demo.auto_rotate = !demo.auto_rotate;
        }
        
        if (IsKeyPressed(KEY_R)) {
            // Fix the RAND_MAX conversion warnings
            demo.base_hue = (float)rand() / (float)RAND_MAX;
            demo.saturation = 0.5f + 0.5f * ((float)rand() / (float)RAND_MAX);
            demo.value = 0.5f + 0.5f * ((float)rand() / (float)RAND_MAX);
            generate_colors(&demo);
        }
        
        // Adjust parameters with keys
        if (IsKeyDown(KEY_H)) demo.base_hue = fmodf(demo.base_hue + delta_time * 0.2f, 1.0f);
        if (IsKeyDown(KEY_S)) demo.saturation = fmaxf(0, fminf(1, demo.saturation + (IsKeyDown(KEY_LEFT_SHIFT) ? -delta_time : delta_time) * 0.5f));
        if (IsKeyDown(KEY_V)) demo.value = fmaxf(0, fminf(1, demo.value + (IsKeyDown(KEY_LEFT_SHIFT) ? -delta_time : delta_time) * 0.5f));
        
        // Auto-rotate hue
        if (demo.auto_rotate) {
            demo.base_hue = fmodf(demo.base_hue + delta_time * 0.1f, 1.0f);
        }
        
        // Regenerate colors if parameters changed
        static float last_hue = -1, last_sat = -1, last_val = -1;
        if (demo.base_hue != last_hue || demo.saturation != last_sat || demo.value != last_val) {
            generate_colors(&demo);
            last_hue = demo.base_hue;
            last_sat = demo.saturation;
            last_val = demo.value;
        }
        
        BeginDrawing();
        ClearBackground(RAYWHITE);
        
        // Draw main visualization
        draw_color_visualization(&demo, 20, 20, SCREEN_WIDTH - 340, 400);
        
        // Draw color swatches
        int swatch_width = 120;
        int swatch_height = 150;
        int swatches_per_row = (SCREEN_WIDTH - 340) / swatch_width;
        
        for(int i = 0; i < demo.color_count; i++) {
            int row = i / swatches_per_row;
            int col = i % swatches_per_row;
            int x = 20 + col * swatch_width;
            int y = 440 + row * swatch_height;
            
            if (y + swatch_height < SCREEN_HEIGHT - 20) {
                draw_color_swatch(demo.colors[i], x, y, swatch_width - 5, swatch_height - 5, i);
            }
        }
        
        // Draw controls panel
        draw_controls(&demo);
        
        // Draw footer
        DrawText("Press SPACE to cycle through color schemes, A to toggle auto-rotate, R for random", 
                10, SCREEN_HEIGHT - 25, 14, DARKGRAY);
        
        EndDrawing();
    }
    
    CloseWindow();
    return 0;
}
