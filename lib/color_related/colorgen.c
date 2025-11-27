// [file name]: colorgen.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define COLORX_IMPLEMENTATION
#include "colorx.h"

#define PALETTEX_IMPLEMENTATION
#include "palettex.h"

// Print usage information
void print_usage(const char* program_name) {
    printf("ColorGen - General Purpose Color Generation Tool\n");
    printf("Usage: %s <command> [options]\n\n", program_name);
    printf("Commands:\n");
    printf("  single    - Generate single color\n");
    printf("  comp      - Generate complementary colors\n");
    printf("  analog    - Generate analogous colors\n");
    printf("  triadic   - Generate triadic colors\n");
    printf("  tetradic  - Generate tetradic colors\n");
    printf("  square    - Generate square colors\n");
    printf("  mono      - Generate monochrome palette\n");
    printf("  even      - Generate evenly spaced colors\n");
    printf("  random    - Generate random palette\n");
    printf("\nOptions:\n");
    printf("  -h <hue>        - Hue (0.0-1.0 or 0-360)\n");
    printf("  -s <saturation> - Saturation (0.0-1.0)\n");
    printf("  -v <value>      - Value/brightness (0.0-1.0)\n");
    printf("  -a <alpha>      - Alpha (0.0-1.0, default: 1.0)\n");
    printf("  -n <count>      - Number of colors (for even/random)\n");
    printf("  -o <offset>     - Offset for analogous (default: 0.08)\n");
    printf("  -f <format>     - Output format: hex, rgb, hsv (default: hex)\n");
    printf("  -p              - Include preview (ASCII art)\n");
    printf("\nExamples:\n");
    printf("  %s single -h 0.5 -s 0.8 -v 0.9\n", program_name);
    printf("  %s comp -h 120 -s 0.7 -v 0.8 -f rgb\n", program_name);
    printf("  %s even -n 8 -s 1.0 -v 0.9 -p\n", program_name);
}

// Convert hue input (accept both 0-1 and 0-360)
float parse_hue(const char* str) {
    float h = atof(str);
    if (h >= 0 && h <= 360) {
        return h / 360.0f; // Convert from degrees to 0-1 range
    }
    return fmodf(h, 1.0f); // Wrap to 0-1 range
}

// Parse float with validation
float parse_float(const char* str, float min_val, float max_val) {
    float val = atof(str);
    if (val < min_val) return min_val;
    if (val > max_val) return max_val;
    return val;
}

// Print color in different formats
void print_color(ColorX c, const char* format, int index) {
    if (index >= 0) {
        printf("%2d: ", index);
    }
    
    if (strcmp(format, "hex") == 0) {
        unsigned hex = cx_to_u32(c);
        printf("#%08X", hex);
    }
    else if (strcmp(format, "rgb") == 0) {
        printf("rgb(%d, %d, %d)", 
               (int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255));
        if (c.a < 1.0f) {
            printf(", alpha: %.2f", c.a);
        }
    }
    else if (strcmp(format, "hsv") == 0) {
        float h, s, v;
        cx_to_hsv(c, &h, &s, &v);
        printf("hsv(%.1f°, %.1f%%, %.1f%%)", h * 360, s * 100, v * 100);
    }
    else {
        // Default to float
        printf("(%.3f, %.3f, %.3f", c.r, c.g, c.b);
        if (c.a < 1.0f) {
            printf(", %.3f", c.a);
        }
        printf(")");
    }
}

// Print ASCII preview of color
void print_preview(ColorX c) {
    printf("  \033[48;2;%d;%d;%dm      \033[0m",
           (int)(c.r * 255), (int)(c.g * 255), (int)(c.b * 255));
}

// Generate and display single color
void cmd_single(float h, float s, float v, float a, const char* format, int preview) {
    ColorX color = cx_from_hsv(h, s, v, a);
    
    printf("Single color:\n");
    print_color(color, format, -1);
    if (preview) {
        print_preview(color);
    }
    printf("\n");
}

// Generate and display complementary colors
void cmd_complementary(float h, float s, float v, const char* format, int preview) {
    ColorX colors[2];
    plt_complementary(colors, h, s, v);
    
    printf("Complementary colors:\n");
    for (int i = 0; i < 2; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display analogous colors
void cmd_analogous(float h, float s, float v, float offset, const char* format, int preview) {
    ColorX colors[3];
    plt_analogous(colors, h, s, v, offset);
    
    printf("Analogous colors (offset: %.3f):\n", offset);
    for (int i = 0; i < 3; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display triadic colors
void cmd_triadic(float h, float s, float v, const char* format, int preview) {
    ColorX colors[3];
    plt_triadic(colors, h, s, v);
    
    printf("Triadic colors:\n");
    for (int i = 0; i < 3; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display tetradic colors
void cmd_tetradic(float h, float s, float v, const char* format, int preview) {
    ColorX colors[4];
    plt_tetradic(colors, h, s, v);
    
    printf("Tetradic colors:\n");
    for (int i = 0; i < 4; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display square colors
void cmd_square(float h, float s, float v, const char* format, int preview) {
    ColorX colors[4];
    plt_square(colors, h, s, v);
    
    printf("Square colors:\n");
    for (int i = 0; i < 4; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display monochrome palette
void cmd_monochrome(float h, float s, float v, const char* format, int preview) {
    ColorX colors[5];
    plt_monochrome(colors, h, s, v);
    
    printf("Monochrome palette:\n");
    for (int i = 0; i < 5; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
}

// Generate and display evenly spaced colors
void cmd_even(int count, float s, float v, const char* format, int preview) {
    ColorX* colors = malloc(count * sizeof(ColorX));
    if (!colors) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
    
    plt_even(colors, count, s, v);
    
    printf("Evenly spaced colors (%d colors):\n", count);
    for (int i = 0; i < count; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
    
    free(colors);
}

// Generate and display random palette
void cmd_random(int count, float s, float v, const char* format, int preview) {
    ColorX* colors = malloc(count * sizeof(ColorX));
    if (!colors) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
    
    plt_random_golden(colors, count, s, v);
    
    printf("Random palette (%d colors, golden ratio):\n", count);
    for (int i = 0; i < count; i++) {
        print_color(colors[i], format, i + 1);
        if (preview) {
            print_preview(colors[i]);
        }
        printf("\n");
    }
    
    free(colors);
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    // Default values
    float h = 0.0f, s = 0.8f, v = 0.8f, a = 1.0f, offset = 0.08f;
    int count = 8;
    const char* format = "hex";
    const char* command = argv[1];
    int preview = 0;

    // Parse command line arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 && i + 1 < argc) {
            h = parse_hue(argv[++i]);
        }
        else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            s = parse_float(argv[++i], 0.0f, 1.0f);
        }
        else if (strcmp(argv[i], "-v") == 0 && i + 1 < argc) {
            v = parse_float(argv[++i], 0.0f, 1.0f);
        }
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            a = parse_float(argv[++i], 0.0f, 1.0f);
        }
        else if (strcmp(argv[i], "-n") == 0 && i + 1 < argc) {
            count = atoi(argv[++i]);
            if (count < 1) count = 1;
            if (count > 100) count = 100;
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            offset = parse_float(argv[++i], 0.0f, 1.0f);
        }
        else if (strcmp(argv[i], "-f") == 0 && i + 1 < argc) {
            format = argv[++i];
        }
        else if (strcmp(argv[i], "-p") == 0) {
            preview = 1;
        }
        else if (strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
    }

    // Execute command
    if (strcmp(command, "single") == 0) {
        cmd_single(h, s, v, a, format, preview);
    }
    else if (strcmp(command, "comp") == 0) {
        cmd_complementary(h, s, v, format, preview);
    }
    else if (strcmp(command, "analog") == 0) {
        cmd_analogous(h, s, v, offset, format, preview);
    }
    else if (strcmp(command, "triadic") == 0) {
        cmd_triadic(h, s, v, format, preview);
    }
    else if (strcmp(command, "tetradic") == 0) {
        cmd_tetradic(h, s, v, format, preview);
    }
    else if (strcmp(command, "square") == 0) {
        cmd_square(h, s, v, format, preview);
    }
    else if (strcmp(command, "mono") == 0) {
        cmd_monochrome(h, s, v, format, preview);
    }
    else if (strcmp(command, "even") == 0) {
        cmd_even(count, s, v, format, preview);
    }
    else if (strcmp(command, "random") == 0) {
        cmd_random(count, s, v, format, preview);
    }
    else {
        fprintf(stderr, "Error: Unknown command '%s'\n", command);
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
