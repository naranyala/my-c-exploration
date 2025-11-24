#ifndef COLOR_UTILS_H
#define COLOR_UTILS_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

// -----------------------------
// Structs
// -----------------------------
typedef struct {
    unsigned char r, g, b;
} RGB;

typedef struct {
    int h;       // 0–360
    int s;       // 0–100
    int l;       // 0–100
} HSL;

// -----------------------------
// Internal helpers
// -----------------------------
static inline unsigned int parse_hex(const char *hex) {
    if (hex[0] == '#') hex++;
    size_t len = strlen(hex);

    char buf[7];
    if (len == 3) {
        // Expand shorthand (#abc → #aabbcc)
        snprintf(buf, sizeof(buf), "%c%c%c%c%c%c",
                 hex[0], hex[0], hex[1], hex[1], hex[2], hex[2]);
        return (unsigned int)strtol(buf, NULL, 16);
    }
    return (unsigned int)strtol(hex, NULL, 16);
}

// -----------------------------
// Color conversion
// -----------------------------
static inline RGB hex_to_rgb(const char *hex) {
    unsigned int value = parse_hex(hex);
    RGB rgb = {
        (value >> 16) & 255,
        (value >> 8) & 255,
        value & 255
    };
    return rgb;
}

static inline void rgb_to_hex(RGB rgb, char *out) {
    sprintf(out, "#%02x%02x%02x", rgb.r, rgb.g, rgb.b);
}

static inline RGB hsl_to_rgb(HSL hsl) {
    double h = hsl.h;
    double s = hsl.s / 100.0;
    double l = hsl.l / 100.0;

    double c = (1 - fabs(2 * l - 1)) * s;
    double x = c * (1 - fabs(fmod(h / 60.0, 2) - 1));
    double m = l - c / 2;

    double r, g, b;
    if (h < 60)      { r = c; g = x; b = 0; }
    else if (h < 120){ r = x; g = c; b = 0; }
    else if (h < 180){ r = 0; g = c; b = x; }
    else if (h < 240){ r = 0; g = x; b = c; }
    else if (h < 300){ r = x; g = 0; b = c; }
    else             { r = c; g = 0; b = x; }

    RGB rgb = {
        (unsigned char)round((r + m) * 255),
        (unsigned char)round((g + m) * 255),
        (unsigned char)round((b + m) * 255)
    };
    return rgb;
}

// -----------------------------
// Color manipulation
// -----------------------------
static inline RGB lighten(RGB rgb, double percent) {
    double factor = 1 + percent / 100.0;
    rgb.r = (unsigned char)fmin(255, round(rgb.r * factor));
    rgb.g = (unsigned char)fmin(255, round(rgb.g * factor));
    rgb.b = (unsigned char)fmin(255, round(rgb.b * factor));
    return rgb;
}

static inline RGB darken(RGB rgb, double percent) {
    double factor = 1 - percent / 100.0;
    rgb.r = (unsigned char)fmax(0, round(rgb.r * factor));
    rgb.g = (unsigned char)fmax(0, round(rgb.g * factor));
    rgb.b = (unsigned char)fmax(0, round(rgb.b * factor));
    return rgb;
}

// -----------------------------
// Color analysis
// -----------------------------
static inline double luminance(RGB rgb) {
    double rs = rgb.r / 255.0;
    double gs = rgb.g / 255.0;
    double bs = rgb.b / 255.0;

    rs = (rs <= 0.03928) ? rs / 12.92 : pow((rs + 0.055) / 1.055, 2.4);
    gs = (gs <= 0.03928) ? gs / 12.92 : pow((gs + 0.055) / 1.055, 2.4);
    bs = (bs <= 0.03928) ? bs / 12.92 : pow((bs + 0.055) / 1.055, 2.4);

    return 0.2126 * rs + 0.7152 * gs + 0.0722 * bs;
}

static inline double contrast(RGB c1, RGB c2) {
    double lum1 = luminance(c1);
    double lum2 = luminance(c2);
    double brightest = fmax(lum1, lum2);
    double darkest   = fmin(lum1, lum2);
    return (brightest + 0.05) / (darkest + 0.05);
}

// -----------------------------
// Random color generation
// -----------------------------
static inline RGB random_rgb() {
    RGB rgb = {
        (unsigned char)(rand() % 256),
        (unsigned char)(rand() % 256),
        (unsigned char)(rand() % 256)
    };
    return rgb;
}

static inline void random_hex(char *out) {
    RGB rgb = random_rgb();
    rgb_to_hex(rgb, out);
}

static inline HSL random_hsl(int hue_min, int hue_max, int saturation, int lightness) {
    HSL hsl = {
        hue_min + rand() % (hue_max - hue_min + 1),
        saturation,
        lightness
    };
    return hsl;
}

#endif // COLOR_UTILS_H

