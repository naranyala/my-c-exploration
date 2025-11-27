#ifndef PALETTEX_H
#define PALETTEX_H

#ifdef __cplusplus
extern "C" {
#endif

// Check if ColorX is already defined (by colorx.h)
#ifndef COLORX_H
// Only define ColorX if colorx.h wasn't included
typedef struct { float r, g, b, a; } ColorX;
#endif

// User must provide this (or include colorx.h)
#ifndef PALETTEX_MAKE_HSV
# include <math.h>
static inline ColorX px_from_hsv(float h, float s, float v, float a) {
    float C = v * s;
    float X = C * (1 - fabsf(fmodf(h*6, 2) - 1));
    float m = v - C;

    float r=0, g=0, b=0;
    float H = h*6;

    if      (H < 1) { r=C; g=X; b=0; }
    else if (H < 2) { r=X; g=C; b=0; }
    else if (H < 3) { r=0; g=C; b=X; }
    else if (H < 4) { r=0; g=X; b=C; }
    else if (H < 5) { r=X; g=0; b=C; }
    else            { r=C; g=0; b=X; }

    return (ColorX){r+m, g+m, b+m, a};
}
#define PALETTEX_MAKE_HSV px_from_hsv
#endif

// ---------------------------------------------------
// PUBLIC API
// ---------------------------------------------------

// Complementary: base + 180°
void plt_complementary(ColorX out[2], float base_h, float s, float v);

// Analogous palette: base ± offset (default offset = 0.08)
void plt_analogous(ColorX out[3], float base_h, float s, float v, float offset);

// Triadic palette: base +120°, +240°
void plt_triadic(ColorX out[3], float base_h, float s, float v);

// Tetradic (rectangle): base +60°, +180°, +240°
void plt_tetradic(ColorX out[4], float base_h, float s, float v);

// Square (every 90°)
void plt_square(ColorX out[4], float base_h, float s, float v);

// Monochrome ladder: dark → mid → light
void plt_monochrome(ColorX out[5], float base_h, float s, float v);

// Evenly spaced hues
void plt_even(ColorX *out, int count, float s, float v);

// Randomized palette based on golden-angle distribution
void plt_random_golden(ColorX *out, int count, float s, float v);


// ---------------------------------------------------
// IMPLEMENTATION
// ---------------------------------------------------
#ifdef PALETTEX_IMPLEMENTATION

static inline float _wrap(float x) {
    if (x < 0) x += 1;
    if (x >= 1) x -= 1;
    return x;
}

void plt_complementary(ColorX out[2], float h, float s, float v) {
    out[0] = PALETTEX_MAKE_HSV(h, s, v, 1.0f);
    out[1] = PALETTEX_MAKE_HSV(_wrap(h + 0.5f), s, v, 1.0f);
}

void plt_analogous(ColorX out[3], float h, float s, float v, float off) {
    out[0] = PALETTEX_MAKE_HSV(_wrap(h - off), s, v, 1.0f);
    out[1] = PALETTEX_MAKE_HSV(h, s, v, 1.0f);
    out[2] = PALETTEX_MAKE_HSV(_wrap(h + off), s, v, 1.0f);
}

void plt_triadic(ColorX out[3], float h, float s, float v) {
    out[0] = PALETTEX_MAKE_HSV(h, s, v, 1.0f);
    out[1] = PALETTEX_MAKE_HSV(_wrap(h + 1.0f/3.0f), s, v, 1.0f);  // FIXED TYPO
    out[2] = PALETTEX_MAKE_HSV(_wrap(h + 2.0f/3.0f), s, v, 1.0f);
}

void plt_tetradic(ColorX out[4], float h, float s, float v) {
    out[0] = PALETTEX_MAKE_HSV(h, s, v, 1.0f);
    out[1] = PALETTEX_MAKE_HSV(_wrap(h + 1.0f/6.0f), s, v, 1.0f);
    out[2] = PALETTEX_MAKE_HSV(_wrap(h + 0.5f), s, v, 1.0f);
    out[3] = PALETTEX_MAKE_HSV(_wrap(h + 4.0f/6.0f), s, v, 1.0f);
}

void plt_square(ColorX out[4], float h, float s, float v) {
    for(int i=0;i<4;i++)
        out[i] = PALETTEX_MAKE_HSV(_wrap(h + i*0.25f), s, v, 1.0f);
}

void plt_monochrome(ColorX out[5], float h, float s, float v) {
    out[0] = PALETTEX_MAKE_HSV(h, s, v*0.25f, 1.0f);
    out[1] = PALETTEX_MAKE_HSV(h, s, v*0.4f, 1.0f);
    out[2] = PALETTEX_MAKE_HSV(h, s, v,       1.0f);
    out[3] = PALETTEX_MAKE_HSV(h, s, fminf(v*1.3f,1), 1.0f);
    out[4] = PALETTEX_MAKE_HSV(h, s, fminf(v*1.6f,1), 1.0f);
}

void plt_even(ColorX *out, int count, float s, float v) {
    float step = 1.0f / count;
    for(int i=0;i<count;i++)
        out[i] = PALETTEX_MAKE_HSV(_wrap(i * step), s, v, 1.0f);
}

void plt_random_golden(ColorX *out, int count, float s, float v) {
    const float golden = 0.61803398875f;  
    float h = 0.37f; 
    for(int i=0;i<count;i++) {
        out[i] = PALETTEX_MAKE_HSV(h, s, v, 1.0f);
        h = _wrap(h + golden);
    }
}

#endif // PALETTEX_IMPLEMENTATION

#ifdef __cplusplus
}
#endif
#endif // PALETTEX_H
