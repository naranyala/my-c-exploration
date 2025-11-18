
#define RGFW_IMPLEMENTATION
#define RGFW_OPENGL
#include "RGFW/RGFW.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>

//
// ─────────────────────────────────────────────
//   Small 8×8 bitmap font (no GLUT needed)
// ─────────────────────────────────────────────
//

static unsigned char font8x8_basic[128][8] = {
    #include "font8x8_basic.h" // I can inline it if you want
};

void drawChar(float x, float y, char c) {
    glBegin(GL_POINTS);
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            if (font8x8_basic[(int)c][row] & (1 << col)) {
                glVertex2f(x + col, y + row);
            }
        }
    }
    glEnd();
}

void drawText(float x, float y, const char* s) {
    for (int i = 0; s[i]; i++)
        drawChar(x + i * 8, y, s[i]);
}

//
// ─────────────────────────────────────────────
//   Very simple calculator logic
// ─────────────────────────────────────────────
//
typedef struct { int x, y, w, h; const char* label; } Button;

#define BTN_ROWS 4
#define BTN_COLS 4
#define BTN_COUNT (BTN_ROWS * BTN_COLS)

Button buttons[BTN_COUNT];
char display[64] = "0";

double stored = 0;
char op = 0;
int reset_display = 0;

void applyOp(double b) {
    double a = stored;

    switch (op) {
        case '+': sprintf(display, "%.10g", a + b); break;
        case '-': sprintf(display, "%.10g", a - b); break;
        case '*': sprintf(display, "%.10g", a * b); break;
        case '/': sprintf(display, "%.10g", (b != 0 ? a / b : 0)); break;
    }
}

//
// ─────────────────────────────────────────────
//   Main
// ─────────────────────────────────────────────
//
int main() {
    RGFW_window* win =
        RGFW_createWindow("RGFW Calculator", 200, 200, 300, 400,
                           RGFW_windowCenter | RGFW_windowOpenGL);

    // Setup coordinate system
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 300, 400, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Build layout
    const char* labels[BTN_COUNT] = {
        "7","8","9","/",
        "4","5","6","*",
        "1","2","3","-",
        "C","0","=","+"
    };
    int bw = 75, bh = 75;
    int idx = 0;

    for (int r = 0; r < BTN_ROWS; r++)
        for (int c = 0; c < BTN_COLS; c++)
            buttons[idx++] = (Button){
                .x = c*bw, .y = 100 + r*bh,
                .w = bw, .h = bh,
                .label = labels[idx-1]
            };

    // Main loop
    while (!RGFW_window_shouldClose(win)) {
        // Events
        RGFW_event e;
        while (RGFW_window_checkEvent(win, &e)) {
            if (e.type == RGFW_mouseButtonPressed) {
                int mx = e.point.x;
                int my = e.point.y;

                for (int i = 0; i < BTN_COUNT; i++) {
                    Button* b = &buttons[i];
                    if (mx >= b->x && mx <= b->x + b->w &&
                        my >= b->y && my <= b->y + b->h) {

                        const char* L = b->label;

                        // Clear
                        if (strcmp(L, "C") == 0) {
                            strcpy(display, "0");
                            op = 0;
                            stored = 0;
                            reset_display = 1;
                            continue;
                        }

                        // Operator
                        if (strchr("+-*/", L[0])) {
                            stored = atof(display);
                            op = L[0];
                            reset_display = 1;
                            continue;
                        }

                        // Equal
                        if (strcmp(L, "=") == 0) {
                            if (op)
                                applyOp(atof(display));
                            op = 0;
                            reset_display = 1;
                            continue;
                        }

                        // Number
                        if (reset_display) {
                            strcpy(display, "");
                            reset_display = 0;
                        }

                        if (strcmp(display, "0") == 0)
                            display[0] = '\0';

                        strcat(display, L);
                    }
                }
            }
        }

        // Render
        glClearColor(0.1,0.1,0.12,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glLoadIdentity();

        // Display area
        glColor3f(0.2,0.2,0.25);
        glBegin(GL_QUADS);
            glVertex2f(0,0);
            glVertex2f(300,0);
            glVertex2f(300,100);
            glVertex2f(0,100);
        glEnd();

        glColor3f(1,1,1);
        drawText(10, 40, display);

        // Buttons
        for (int i = 0; i < BTN_COUNT; i++) {
            Button* b = &buttons[i];
            glColor3f(0.25,0.25,0.3);
            glBegin(GL_QUADS);
                glVertex2f(b->x + 5,       b->y + 5);
                glVertex2f(b->x + b->w-5,  b->y + 5);
                glVertex2f(b->x + b->w-5,  b->y + b->h-5);
                glVertex2f(b->x + 5,       b->y + b->h-5);
            glEnd();

            glColor3f(1,1,1);
            drawText(b->x + 30, b->y + 40, b->label);
        }

        RGFW_window_swapBuffers_OpenGL(win);
    }

    RGFW_window_close(win);
    return 0;
}
