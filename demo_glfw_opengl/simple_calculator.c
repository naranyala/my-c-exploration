/*
 * Simple Calculator using GLFW + OpenGL + stb_truetype
 *
 * Build instructions:
 * Linux:   gcc calculator.c -o calculator -lglfw -lGL -lm
 * macOS:   clang calculator.c -o calculator -lglfw -framework OpenGL
 * Windows: cl calculator.c /link glfw3.lib opengl32.lib
 *
 * Requires:
 * - GLFW3 library
 * - stb_truetype.h (download from https://github.com/nothings/stb)
 * - A TrueType font file (e.g., Arial.ttf, DejaVuSans.ttf)
 */

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_DISPLAY 32
#define WINDOW_WIDTH 330
#define WINDOW_HEIGHT 380
#define FONT_SIZE 24.0f
#define BUTTON_FONT_SIZE 20.0f

typedef struct {
    char display[MAX_DISPLAY];
    double current_value;
    double stored_value;
    char operation;
    int new_number;
} calculator_state_t;

typedef struct {
    float x, y, w, h;
    const char* label;
    char value;
} button_t;

// Font data
static stbtt_fontinfo font;
static unsigned char* font_buffer = NULL;
static unsigned char* temp_bitmap = NULL;

static calculator_state_t calc = {0};
static int window_width = WINDOW_WIDTH;
static int window_height = WINDOW_HEIGHT;

static button_t buttons[] = {
    {10, 80, 70, 50, "7", '7'},
    {90, 80, 70, 50, "8", '8'},
    {170, 80, 70, 50, "9", '9'},
    {250, 80, 70, 50, "/", '/'},

    {10, 140, 70, 50, "4", '4'},
    {90, 140, 70, 50, "5", '5'},
    {170, 140, 70, 50, "6", '6'},
    {250, 140, 70, 50, "*", '*'},

    {10, 200, 70, 50, "1", '1'},
    {90, 200, 70, 50, "2", '2'},
    {170, 200, 70, 50, "3", '3'},
    {250, 200, 70, 50, "-", '-'},

    {10, 260, 70, 50, "0", '0'},
    {90, 260, 70, 50, ".", '.'},
    {170, 260, 70, 50, "=", '='},
    {250, 260, 70, 50, "+", '+'},

    {10, 320, 310, 50, "Clear", 'C'},
};

static const int num_buttons = sizeof(buttons) / sizeof(buttons[0]);

int load_font(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        // Try common font locations
        const char* font_paths[] = {
            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
            "/System/Library/Fonts/Helvetica.ttc",
            "C:\\Windows\\Fonts\\arial.ttf",
            "/usr/share/fonts/TTF/DejaVuSans.ttf",
            "DejaVuSans.ttf",
            "arial.ttf",
            NULL
        };

        for (int i = 0; font_paths[i] != NULL; i++) {
            f = fopen(font_paths[i], "rb");
            if (f) {
                printf("Loaded font: %s\n", font_paths[i]);
                break;
            }
        }

        if (!f) {
            fprintf(stderr, "Could not find any TrueType font file\n");
            return 0;
        }
    }

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);
    fseek(f, 0, SEEK_SET);

    font_buffer = (unsigned char*)malloc(size);
    fread(font_buffer, 1, size, f);
    fclose(f);

    if (!stbtt_InitFont(&font, font_buffer, 0)) {
        fprintf(stderr, "Failed to initialize font\n");
        free(font_buffer);
        return 0;
    }

    temp_bitmap = (unsigned char*)malloc(512 * 512);
    return 1;
}

void calc_init(void) {
    strcpy(calc.display, "0");
    calc.current_value = 0;
    calc.stored_value = 0;
    calc.operation = 0;
    calc.new_number = 1;
}

void calc_append_digit(char digit) {
    if (calc.new_number) {
        if (digit == '.') {
            strcpy(calc.display, "0.");
        } else {
            calc.display[0] = digit;
            calc.display[1] = '\0';
        }
        calc.new_number = 0;
    } else {
        size_t len = strlen(calc.display);
        if (len < MAX_DISPLAY - 1) {
            if (digit == '.' && strchr(calc.display, '.')) {
                return;
            }
            calc.display[len] = digit;
            calc.display[len + 1] = '\0';
        }
    }
}

double calc_perform_operation(double a, double b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b != 0 ? a / b : 0;
        default: return b;
    }
}

void calc_button_press(char btn) {
    if (btn >= '0' && btn <= '9') {
        calc_append_digit(btn);
    } else if (btn == '.') {
        calc_append_digit(btn);
    } else if (btn == '+' || btn == '-' || btn == '*' || btn == '/') {
        calc.current_value = atof(calc.display);
        if (calc.operation && !calc.new_number) {
            calc.stored_value = calc_perform_operation(calc.stored_value, calc.current_value, calc.operation);
            snprintf(calc.display, MAX_DISPLAY, "%.8g", calc.stored_value);
        } else {
            calc.stored_value = calc.current_value;
        }
        calc.operation = btn;
        calc.new_number = 1;
    } else if (btn == '=') {
        calc.current_value = atof(calc.display);
        if (calc.operation) {
            calc.stored_value = calc_perform_operation(calc.stored_value, calc.current_value, calc.operation);
            snprintf(calc.display, MAX_DISPLAY, "%.8g", calc.stored_value);
            calc.operation = 0;
        }
        calc.new_number = 1;
    } else if (btn == 'C') {
        calc_init();
    }
}

void draw_rect(float x, float y, float w, float h) {
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void draw_rect_outline(float x, float y, float w, float h) {
    glBegin(GL_LINE_LOOP);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void draw_text(float x, float y, const char* text, float font_size) {
    float scale = stbtt_ScaleForPixelHeight(&font, font_size);

    int ascent, descent, lineGap;
    stbtt_GetFontVMetrics(&font, &ascent, &descent, &lineGap);

    float baseline = y + ascent * scale;
    float xpos = x;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (int i = 0; text[i] != '\0'; i++) {
        int advance, lsb;
        int x0, y0, x1, y1;

        stbtt_GetCodepointHMetrics(&font, text[i], &advance, &lsb);
        stbtt_GetCodepointBitmapBox(&font, text[i], scale, scale, &x0, &y0, &x1, &y1);

        int w = x1 - x0;
        int h = y1 - y0;

        if (w > 0 && h > 0) {
            stbtt_MakeCodepointBitmap(&font, temp_bitmap, w, h, w, scale, scale, text[i]);

            float char_x = xpos + x0;
            float char_y = baseline + y0;

            glBegin(GL_QUADS);
            for (int py = 0; py < h; py++) {
                for (int px = 0; px < w; px++) {
                    unsigned char alpha = temp_bitmap[py * w + px];
                    if (alpha > 20) {
                        float intensity = alpha / 255.0f;
                        glColor4f(0.0f, 0.0f, 0.0f, intensity);

                        float fx = char_x + px;
                        float fy = char_y + py;
                        glVertex2f(fx, fy);
                        glVertex2f(fx + 1, fy);
                        glVertex2f(fx + 1, fy + 1);
                        glVertex2f(fx, fy + 1);
                    }
                }
            }
            glEnd();
        }

        xpos += advance * scale;

        if (text[i + 1]) {
            xpos += scale * stbtt_GetCodepointKernAdvance(&font, text[i], text[i + 1]);
        }
    }

    glDisable(GL_BLEND);
}

float measure_text(const char* text, float font_size) {
    float scale = stbtt_ScaleForPixelHeight(&font, font_size);
    float width = 0;

    for (int i = 0; text[i] != '\0'; i++) {
        int advance, lsb;
        stbtt_GetCodepointHMetrics(&font, text[i], &advance, &lsb);
        width += advance * scale;

        if (text[i + 1]) {
            width += scale * stbtt_GetCodepointKernAdvance(&font, text[i], text[i + 1]);
        }
    }

    return width;
}

void render(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, window_width, window_height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Draw display background
    glColor3f(1.0f, 1.0f, 1.0f);
    draw_rect(10, 10, 310, 60);

    glColor3f(0.95f, 0.95f, 0.95f);
    draw_rect(15, 15, 300, 50);

    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(1.0f);
    draw_rect_outline(10, 10, 310, 60);

    // Draw display text (right-aligned)
    float text_width = measure_text(calc.display, FONT_SIZE);
    draw_text(305 - text_width, 28, calc.display, FONT_SIZE);

    // Draw buttons
    for (int i = 0; i < num_buttons; i++) {
        button_t* btn = &buttons[i];

        // Button background
        glColor3f(0.75f, 0.75f, 0.75f);
        draw_rect(btn->x, btn->y, btn->w, btn->h);

        // Button face
        glColor3f(0.92f, 0.92f, 0.92f);
        draw_rect(btn->x + 2, btn->y + 2, btn->w - 4, btn->h - 4);

        // Button outline
        glColor3f(0.5f, 0.5f, 0.5f);
        glLineWidth(1.0f);
        draw_rect_outline(btn->x, btn->y, btn->w, btn->h);

        // Button label (centered)
        float label_width = measure_text(btn->label, BUTTON_FONT_SIZE);
        float label_x = btn->x + (btn->w - label_width) / 2;
        float label_y = btn->y + (btn->h - BUTTON_FONT_SIZE) / 2;
        draw_text(label_x, label_y, btn->label, BUTTON_FONT_SIZE);
    }
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        for (int i = 0; i < num_buttons; i++) {
            button_t* btn = &buttons[i];
            if (xpos >= btn->x && xpos <= btn->x + btn->w &&
                ypos >= btn->y && ypos <= btn->y + btn->h) {
                calc_button_press(btn->value);
                break;
            }
        }
    }
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (action == GLFW_PRESS || action == GLFW_REPEAT) {
        if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9) {
            calc_button_press('0' + (key - GLFW_KEY_0));
        } else if (key >= GLFW_KEY_KP_0 && key <= GLFW_KEY_KP_9) {
            calc_button_press('0' + (key - GLFW_KEY_KP_0));
        } else if (key == GLFW_KEY_EQUAL || key == GLFW_KEY_KP_ADD) {
            calc_button_press('+');
        } else if (key == GLFW_KEY_MINUS || key == GLFW_KEY_KP_SUBTRACT) {
            calc_button_press('-');
        } else if ((key == GLFW_KEY_8 && (mods & GLFW_MOD_SHIFT)) || key == GLFW_KEY_KP_MULTIPLY) {
            calc_button_press('*');
        } else if (key == GLFW_KEY_SLASH || key == GLFW_KEY_KP_DIVIDE) {
            calc_button_press('/');
        } else if (key == GLFW_KEY_PERIOD || key == GLFW_KEY_KP_DECIMAL) {
            calc_button_press('.');
        } else if (key == GLFW_KEY_ENTER || key == GLFW_KEY_KP_ENTER) {
            calc_button_press('=');
        } else if (key == GLFW_KEY_ESCAPE || key == GLFW_KEY_C) {
            calc_button_press('C');
        }
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    window_width = width;
    window_height = height;
    glViewport(0, 0, width, height);
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Calculator - stb_truetype", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Load font
    if (!load_font(NULL)) {
        fprintf(stderr, "Failed to load font\n");
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    calc_init();
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    while (!glfwWindowShouldClose(window)) {
        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    free(font_buffer);
    free(temp_bitmap);
    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
