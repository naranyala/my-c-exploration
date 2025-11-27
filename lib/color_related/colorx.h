
#ifndef COLORX_H
#define COLORX_H

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------
// Public API
// ----------------------------------

typedef struct {
    float r, g, b, a;
} ColorX;

// Constructors
static inline ColorX cx_rgba(float r, float g, float b, float a);
static inline ColorX cx_rgb(float r, float g, float b);
static inline ColorX cx_gray(float v);

// Ops
static inline ColorX cx_add(ColorX a, ColorX b);
static inline ColorX cx_sub(ColorX a, ColorX b);
static inline ColorX cx_mul(ColorX a, ColorX b);
static inline ColorX cx_scale(ColorX c, float s);
static inline ColorX cx_lerp(ColorX a, ColorX b, float t);
static inline ColorX cx_clamp(ColorX c);

// Color models
static inline ColorX cx_from_hsv(float h, float s, float v, float a);
static inline void   cx_to_hsv(ColorX c, float *h, float *s, float *v);

// Utilities
static inline unsigned cx_to_u32(ColorX c);
static inline ColorX   cx_from_u32(unsigned p);

// Presets
#define CX_WHITE  (ColorX){1,1,1,1}
#define CX_BLACK  (ColorX){0,0,0,1}
#define CX_RED    (ColorX){1,0,0,1}
#define CX_GREEN  (ColorX){0,1,0,1}
#define CX_BLUE   (ColorX){0,0,1,1}
#define CX_YELLOW (ColorX){1,1,0,1}
#define CX_CYAN   (ColorX){0,1,1,1}
#define CX_MAGENTA (ColorX){1,0,1,1}


// ----------------------------------
// Implementation
// ----------------------------------
#ifdef COLORX_IMPLEMENTATION

// --- Constructors ---
static inline ColorX cx_rgba(float r, float g, float b, float a) {
    ColorX c = {r, g, b, a};
    return c;
}
static inline ColorX cx_rgb(float r, float g, float b) {
    ColorX c = {r, g, b, 1.0f};
    return c;
}
static inline ColorX cx_gray(float v) {
    ColorX c = {v, v, v, 1.0f};
    return c;
}

// --- Arithmetic ---
static inline ColorX cx_add(ColorX a, ColorX b) {
    return (ColorX){a.r+b.r, a.g+b.g, a.b+b.b, a.a+b.a};
}
static inline ColorX cx_sub(ColorX a, ColorX b) {
    return (ColorX){a.r-b.r, a.g-b.g, a.b-b.b, a.a-b.a};
}
static inline ColorX cx_mul(ColorX a, ColorX b) {
    return (ColorX){a.r*b.r, a.g*b.g, a.b*b.b, a.a*b.a};
}
static inline ColorX cx_scale(ColorX c, float s) {
    return (ColorX){c.r*s, c.g*s, c.b*s, c.a*s};
}

static inline ColorX cx_clamp(ColorX c) {
    #define CL(x) (x<0?0:(x>1?1:x))
    return (ColorX){CL(c.r), CL(c.g), CL(c.b), CL(c.a)};
    #undef CL
}

static inline ColorX cx_lerp(ColorX a, ColorX b, float t) {
    return (ColorX){
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t,
        a.a + (b.a - a.a) * t
    };
}

// --- Packed 32-bit conversion ---
static inline unsigned cx_to_u32(ColorX c) {
    unsigned r = (unsigned)(c.r * 255.0f);
    unsigned g = (unsigned)(c.g * 255.0f);
    unsigned b = (unsigned)(c.b * 255.0f);
    unsigned a = (unsigned)(c.a * 255.0f);
    return (a<<24) | (b<<16) | (g<<8) | r;
}

static inline ColorX cx_from_u32(unsigned p) {
    return (ColorX){
        (p & 0xFF) / 255.0f,
        ((p >> 8) & 0xFF) / 255.0f,
        ((p >> 16) & 0xFF) / 255.0f,
        ((p >> 24) & 0xFF) / 255.0f
    };
}

// --- HSV conversion ---
static inline ColorX cx_from_hsv(float h, float s, float v, float a) {
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

static inline void cx_to_hsv(ColorX c, float *h, float *s, float *v) {
    float maxv = fmaxf(c.r, fmaxf(c.g, c.b));
    float minv = fminf(c.r, fminf(c.g, c.b));
    float d = maxv - minv;

    *v = maxv;

    if (d < 0.00001f) {
        *h = 0;
        *s = 0;
        return;
    }

    *s = (maxv == 0) ? 0 : (d / maxv);

    if (maxv == c.r)       *h = fmodf((c.g - c.b) / d, 6) / 6.0f;
    else if (maxv == c.g)  *h = ((c.b - c.r) / d + 2) / 6.0f;
    else                   *h = ((c.r - c.g) / d + 4) / 6.0f;

    if (*h < 0) *h += 1;
}

#endif // COLORX_IMPLEMENTATION

#ifdef __cplusplus
}
#endif
#endif // COLORX_H
