
#define SOKOL_IMPL
#define SOKOL_GL_IMPL
#define SOKOL_GLCORE33
#include "sokol/sokol_app.h"
#include "sokol/sokol_gfx.h"
#include "sokol/sokol_gl.h"
#include "sokol/sokol_glue.h"

#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_DISPLAY 32

typedef struct {
    char display[MAX_DISPLAY];
    double current_value;
    double stored_value;
    char operation;
    bool new_number;
} calculator_state_t;

static calculator_state_t calc = {0};
static sg_pass_action pass_action;

// Button layout
typedef struct {
    float x, y, w, h;
    const char* label;
    char value;
} button_t;

static button_t buttons[] = {
    // Row 1
    {10, 80, 70, 50, "7", '7'},
    {90, 80, 70, 50, "8", '8'},
    {170, 80, 70, 50, "9", '9'},
    {250, 80, 70, 50, "/", '/'},

    // Row 2
    {10, 140, 70, 50, "4", '4'},
    {90, 140, 70, 50, "5", '5'},
    {170, 140, 70, 50, "6", '6'},
    {250, 140, 70, 50, "*", '*'},

    // Row 3
    {10, 200, 70, 50, "1", '1'},
    {90, 200, 70, 50, "2", '2'},
    {170, 200, 70, 50, "3", '3'},
    {250, 200, 70, 50, "-", '-'},

    // Row 4
    {10, 260, 70, 50, "0", '0'},
    {90, 260, 70, 50, ".", '.'},
    {170, 260, 70, 50, "=", '='},
    {250, 260, 70, 50, "+", '+'},

    // Clear button
    {10, 320, 310, 50, "Clear", 'C'},
};

static const int num_buttons = sizeof(buttons) / sizeof(buttons[0]);

void calc_init(void) {
    strcpy(calc.display, "0");
    calc.current_value = 0;
    calc.stored_value = 0;
    calc.operation = 0;
    calc.new_number = true;
}

void calc_append_digit(char digit) {
    if (calc.new_number) {
        if (digit == '.') {
            strcpy(calc.display, "0.");
        } else {
            calc.display[0] = digit;
            calc.display[1] = '\0';
        }
        calc.new_number = false;
    } else {
        size_t len = strlen(calc.display);
        if (len < MAX_DISPLAY - 1) {
            if (digit == '.' && strchr(calc.display, '.')) {
                return; // Already has decimal point
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
        calc.new_number = true;
    } else if (btn == '=') {
        calc.current_value = atof(calc.display);
        if (calc.operation) {
            calc.stored_value = calc_perform_operation(calc.stored_value, calc.current_value, calc.operation);
            snprintf(calc.display, MAX_DISPLAY, "%.8g", calc.stored_value);
            calc.operation = 0;
        }
        calc.new_number = true;
    } else if (btn == 'C') {
        calc_init();
    }
}

void draw_rect(float x, float y, float w, float h, float r, float g, float b) {
    sgl_begin_quads();
    sgl_c3f(r, g, b);
    sgl_v2f(x, y);
    sgl_v2f(x + w, y);
    sgl_v2f(x + w, y + h);
    sgl_v2f(x, y + h);
    sgl_end();
}

void draw_text(float x, float y, const char* text, float size) {
    // Simple character rendering (very basic)
    // In a real app, you'd use sokol_debugtext or a proper font renderer
    (void)x; (void)y; (void)text; (void)size;
    // Placeholder - actual text rendering would need a font system
}

void init(void) {
    sg_setup(&(sg_desc){
        .context = sapp_sgcontext()
    });

    sgl_setup(&(sgl_desc_t){0});

    pass_action = (sg_pass_action) {
        .colors[0] = { .load_action = SG_LOADACTION_CLEAR, .clear_value = {0.2f, 0.2f, 0.2f, 1.0f} }
    };

    calc_init();
}

void frame(void) {
    int width = sapp_width();
    int height = sapp_height();

    sgl_defaults();
    sgl_matrix_mode_projection();
    sgl_ortho(0.0f, (float)width, (float)height, 0.0f, -1.0f, 1.0f);

    // Draw display
    draw_rect(10, 10, 310, 60, 1.0f, 1.0f, 1.0f);
    draw_rect(15, 15, 300, 50, 0.9f, 0.9f, 0.9f);

    // Draw buttons
    for (int i = 0; i < num_buttons; i++) {
        button_t* btn = &buttons[i];
        draw_rect(btn->x, btn->y, btn->w, btn->h, 0.7f, 0.7f, 0.7f);
        draw_rect(btn->x + 2, btn->y + 2, btn->w - 4, btn->h - 4, 0.85f, 0.85f, 0.85f);
    }

    sg_begin_default_pass(&pass_action, width, height);
    sgl_draw();
    sg_end_pass();
    sg_commit();
}

void event(const sapp_event* e) {
    if (e->type == SAPP_EVENTTYPE_MOUSE_DOWN) {
        float mx = e->mouse_x;
        float my = e->mouse_y;

        for (int i = 0; i < num_buttons; i++) {
            button_t* btn = &buttons[i];
            if (mx >= btn->x && mx <= btn->x + btn->w &&
                my >= btn->y && my <= btn->y + btn->h) {
                calc_button_press(btn->value);
                printf("Display: %s\n", calc.display);
                break;
            }
        }
    } else if (e->type == SAPP_EVENTTYPE_KEY_DOWN) {
        char key = (char)e->key_code;
        if ((key >= '0' && key <= '9') || key == '+' || key == '-' ||
            key == '*' || key == '/' || key == '.') {
            calc_button_press(key);
        } else if (key == SAPP_KEYCODE_ENTER) {
            calc_button_press('=');
        } else if (key == SAPP_KEYCODE_ESCAPE || key == 'C' || key == 'c') {
            calc_button_press('C');
        }
        printf("Display: %s\n", calc.display);
    }
}

void cleanup(void) {
    sgl_shutdown();
    sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
    (void)argc; (void)argv;
    return (sapp_desc){
        .init_cb = init,
        .frame_cb = frame,
        .event_cb = event,
        .cleanup_cb = cleanup,
        .width = 330,
        .height = 380,
        .window_title = "Sokol Calculator",
    };
}
