#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

typedef struct {
    bool visible;
    char title[64];
    char message[256];
    int x, y;
    int width, height;
    bool has_ok;
    bool has_cancel;
    bool result; // true for OK, false for Cancel
} ModalPopup;

typedef struct {
    GLFWwindow* window;
    ModalPopup popup;
    bool show_popup;
    float bg_color[3];
    double mouse_x, mouse_y;
    bool mouse_pressed;
} AppState;

// Initialize modal popup
void init_popup(ModalPopup* popup, const char* title, const char* message, 
                bool has_ok, bool has_cancel) {
    popup->visible = false;
    strncpy(popup->title, title, sizeof(popup->title) - 1);
    strncpy(popup->message, message, sizeof(popup->message) - 1);
    popup->has_ok = has_ok;
    popup->has_cancel = has_cancel;
    popup->width = 300;
    popup->height = 150;
    popup->x = 0;
    popup->y = 0;
}

// Show modal popup
void show_popup(ModalPopup* popup, int parent_width, int parent_height) {
    popup->visible = true;
    popup->x = (parent_width - popup->width) / 2;
    popup->y = (parent_height - popup->height) / 2;
}

// Draw a rectangle with border
void draw_rect(float x, float y, float width, float height, 
               float r, float g, float b, float border_r, float border_g, float border_b) {
    // Draw border
    glColor3f(border_r, border_g, border_b);
    glBegin(GL_QUADS);
    glVertex2f(x - 2, y - 2);
    glVertex2f(x + width + 2, y - 2);
    glVertex2f(x + width + 2, y + height + 2);
    glVertex2f(x - 2, y + height + 2);
    glEnd();
    
    // Draw background
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}

// Very basic text rendering using OpenGL primitives
void draw_text(float x, float y, const char* text, float r, float g, float b) {
    glColor3f(r, g, b);
    glRasterPos2f(x, y);
    
    // Simple implementation: draw the text as a series of lines
    // This is a very basic font - in production, you'd want a proper font system
    float current_x = x;
    
    for (const char* c = text; *c != '\0'; c++) {
        switch (*c) {
            case 'A': case 'a':
                glBegin(GL_LINES);
                glVertex2f(current_x, y); glVertex2f(current_x + 8, y + 12);
                glVertex2f(current_x + 8, y + 12); glVertex2f(current_x + 16, y);
                glVertex2f(current_x + 3, y + 6); glVertex2f(current_x + 13, y + 6);
                glEnd();
                break;
            case 'B': case 'b':
                glBegin(GL_LINE_STRIP);
                glVertex2f(current_x, y); glVertex2f(current_x, y + 12);
                glVertex2f(current_x + 12, y + 10); glVertex2f(current_x, y + 6);
                glVertex2f(current_x + 12, y + 4); glVertex2f(current_x, y);
                glEnd();
                break;
            case 'C': case 'c':
                glBegin(GL_LINE_STRIP);
                glVertex2f(current_x + 12, y + 12); glVertex2f(current_x, y + 6);
                glVertex2f(current_x + 12, y);
                glEnd();
                break;
            // Add more characters as needed, but for demo we'll use simple rectangles
            default:
                // Draw a simple rectangle for unknown characters
                draw_rect(current_x, y, 8, 12, r, g, b, r, g, b);
                break;
        }
        current_x += 10; // Move to next character position
    }
}

// Draw button and return true if clicked
bool draw_button(float x, float y, float width, float height, const char* text, 
                 double mouse_x, double mouse_y, bool mouse_pressed) {
    bool hovered = (mouse_x >= x && mouse_x <= x + width &&
                   mouse_y >= y && mouse_y <= y + height);
    
    // Button colors based on state
    float r, g, b;
    if (hovered && mouse_pressed) {
        r = 0.3f; g = 0.5f; b = 0.8f; // Darker when pressed
    } else if (hovered) {
        r = 0.5f; g = 0.7f; b = 1.0f; // Brighter when hovered
    } else {
        r = 0.4f; g = 0.6f; b = 0.9f; // Normal
    }
    
    draw_rect(x, y, width, height, r, g, b, 0.2f, 0.2f, 0.2f);
    
    // Center text in button (very basic centering)
    float text_x = x + (width - strlen(text) * 8) / 2;
    float text_y = y + height / 2 - 6;
    draw_text(text_x, text_y, text, 1.0f, 1.0f, 1.0f);
    
    return hovered && mouse_pressed;
}

// Draw modal popup
void draw_popup(ModalPopup* popup, double mouse_x, double mouse_y, bool mouse_pressed) {
    if (!popup->visible) return;
    
    // Dim background with semi-transparent overlay
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 0.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0);
    glVertex2f(800, 0); // Assuming 800x600 window
    glVertex2f(800, 600);
    glVertex2f(0, 600);
    glEnd();
    glDisable(GL_BLEND);
    
    // Popup background
    draw_rect(popup->x, popup->y, popup->width, popup->height, 
              0.95f, 0.95f, 0.95f, 0.3f, 0.3f, 0.3f);
    
    // Title bar
    draw_rect(popup->x, popup->y, popup->width, 30, 
              0.2f, 0.4f, 0.8f, 0.1f, 0.1f, 0.1f);
    
    // Title text
    draw_text(popup->x + 10, popup->y + 20, popup->title, 1.0f, 1.0f, 1.0f);
    
    // Message text
    draw_text(popup->x + 20, popup->y + 70, popup->message, 0.0f, 0.0f, 0.0f);
    
    // Buttons
    float button_y = popup->y + popup->height - 40;
    float button_width = 80;
    float button_height = 30;
    float button_spacing = 20;
    
    float total_buttons_width = 0;
    if (popup->has_ok) total_buttons_width += button_width;
    if (popup->has_cancel) total_buttons_width += button_width + button_spacing;
    
    float buttons_start_x = popup->x + (popup->width - total_buttons_width) / 2;
    
    int button_index = 0;
    
    if (popup->has_ok) {
        float button_x = buttons_start_x + button_index * (button_width + button_spacing);
        
        if (draw_button(button_x, button_y, button_width, button_height, "OK", 
                       mouse_x, mouse_y, mouse_pressed)) {
            popup->result = true;
            popup->visible = false;
            printf("OK button clicked!\n");
        }
        button_index++;
    }
    
    if (popup->has_cancel) {
        float button_x = buttons_start_x + button_index * (button_width + button_spacing);
        
        if (draw_button(button_x, button_y, button_width, button_height, "Cancel", 
                       mouse_x, mouse_y, mouse_pressed)) {
            popup->result = false;
            popup->visible = false;
            printf("Cancel button clicked!\n");
        }
    }
}

// Mouse callback
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    AppState* state = (AppState*)glfwGetWindowUserPointer(window);
    
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        state->mouse_pressed = (action == GLFW_PRESS);
        
        if (action == GLFW_PRESS && !state->popup.visible) {
            // Show different popups based on click position
            if (state->mouse_x < 400) { // Left side - info popup
                init_popup(&state->popup, "Information", "This is an informational message!", true, false);
            } else { // Right side - confirmation popup
                init_popup(&state->popup, "Confirmation", "Are you sure you want to continue?", true, true);
            }
            
            int width, height;
            glfwGetWindowSize(window, &width, &height);
            show_popup(&state->popup, width, height);
        }
    }
}

// Cursor position callback
void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    AppState* state = (AppState*)glfwGetWindowUserPointer(window);
    state->mouse_x = xpos;
    state->mouse_y = ypos;
}

// Window resize callback
void window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }
    
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW Modal Popup Demo", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    // Initialize application state
    AppState state = {0};
    state.window = window;
    state.bg_color[0] = 0.2f;
    state.bg_color[1] = 0.3f;
    state.bg_color[2] = 0.4f;
    
    glfwSetWindowUserPointer(window, &state);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);
    
    // Set up orthographic projection
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    window_size_callback(window, width, height);
    
    // Enable line smoothing for better looking UI
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    while (!glfwWindowShouldClose(window)) {
        glClearColor(state.bg_color[0], state.bg_color[1], state.bg_color[2], 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Draw instructions
        if (!state.popup.visible) {
            draw_text(50, 50, "Click on left side for Info Popup", 1.0f, 1.0f, 1.0f);
            draw_text(50, 80, "Click on right side for Confirmation Popup", 1.0f, 1.0f, 1.0f);
            draw_text(50, 110, "Popup result will be printed to console", 1.0f, 1.0f, 1.0f);
        }
        
        // Draw popup (if visible)
        draw_popup(&state.popup, state.mouse_x, state.mouse_y, state.mouse_pressed);
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}
