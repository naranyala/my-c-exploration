
// main.c
// Static article preview using C + GLFW + OpenGL 2.1 + stb_easy_font
// Save stb_easy_font.h in the same directory.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GLFW/glfw3.h>

#define PAGE_W 1024
#define PAGE_H 768

// Forward decls
void draw_rect(float x, float y, float w, float h, float r, float g, float b);
void begin_ortho(int w, int h);
void end_ortho(void);
void draw_text(float x, float y, const char *text, float r, float g, float b);
void draw_paragraph(float x, float y, float max_w, float line_h, const char *text, float r, float g, float b);
void wrap_and_draw(float x, float y, float max_w, float line_h, const char *text, float r, float g, float b);

#define STB_EASY_FONT_IMPLEMENTATION
#include "stb_easy_font.h"

// Approximate character width for stb_easy_font (pixels)
static const float CHAR_W = 7.0f;

// Basic UI helpers
void draw_rect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x,     y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x,     y + h);
    glEnd();
}

void begin_ortho(int w, int h) {
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, w, h, 0, -1, 1);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glDisable(GL_DEPTH_TEST);
}

void end_ortho(void) {
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
}

// Text drawing using stb_easy_font (GL_QUADS client arrays)
void draw_text(float x, float y, const char *text, float r, float g, float b) {
    char buffer[64 * 1024];
    int num_quads = stb_easy_font_print(x, y, (char*)text, NULL, buffer, sizeof(buffer));

    glColor3f(r, g, b);
    glEnableClientState(GL_VERTEX_ARRAY);
    // 2D positions, stride 16 bytes (each vertex is {x,y,z,w}? stb stores 4 floats per vertex? It uses 4 floats per vertex quad entry)
    glVertexPointer(2, GL_FLOAT, 16, buffer);
    glDrawArrays(GL_QUADS, 0, num_quads * 4);
    glDisableClientState(GL_VERTEX_ARRAY);
}

// Naive word-wrapping and drawing
void wrap_and_draw(float x, float y, float max_w, float line_h, const char *text, float r, float g, float b) {
    const int max_chars = (int)floorf(max_w / CHAR_W);
    if (max_chars <= 1) { draw_text(x, y, text, r, g, b); return; }

    const char *p = text;
    char line[2048];
    int line_len = 0;
    float cy = y;

    while (*p) {
        // Skip leading spaces/newlines
        while (*p == ' ') p++;
        if (*p == '\n') { p++; continue; }

        // Fill a line up to max_chars, backtrack to last space
        const char *start = p;
        int count = 0;
        int last_space = -1;

        while (*p && *p != '\n' && count < max_chars) {
            if (*p == ' ') last_space = count;
            line[count++] = *p++;
        }

        if (*p && *p != '\n' && count >= max_chars && last_space != -1) {
            // Backtrack to last space
            int keep = last_space;
            // Rewind p by (count - keep)
            p -= (count - keep);
            count = keep;
        }

        // Trim trailing spaces
        while (count > 0 && line[count - 1] == ' ') count--;

        line[count] = '\0';
        draw_text(x, cy, line, r, g, b);
        cy += line_h;

        // If we hit a newline, advance and continue
        if (*p == '\n') { p++; }
    }
}

void draw_paragraph(float x, float y, float max_w, float line_h, const char *text, float r, float g, float b) {
    wrap_and_draw(x, y, max_w, line_h, text, r, g, b);
}

int main(void) {
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return 1;
    }

    // Request a compatibility profile where immediate-mode works
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);

    GLFWwindow* window = glfwCreateWindow(PAGE_W, PAGE_H, "Article Preview (Static)", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    // Content
    const char *title =
        "Magnetic inductive phase shift: a new method to differentiate hemorrhagic stroke from ischemic stroke on rabbit";

    const char *authors =
        "Qingguang Yan, Gui Jin, Ke Ma, Mingxin Qin, Wei Zhuang, Jian Sun";

    const char *affiliation =
        "College of Biomedical Engineering, Third Military Medical University, Chongqing, China";

    const char *abstract_hdr = "Abstract";
    const char *abstract_bg =
        "Background: The major therapy for ischemic stroke is thrombolytic treatment, but severe consequences occur when "
        "this method is used to treat hemorrhagic stroke. Currently, computed tomography and magnetic resonance imaging "
        "are used to differentiate between two types of stroke, but these two methods are ineffective for pre-hospital care.";

    const char *abstract_methods =
        "Methods: We developed a new brain diagnostic device for rabbits based on electromagnetic induction to non-invasively "
        "differentiate two types of stroke. The device includes two coils and a phase difference measurement system that detects "
        "the magnetic inductive phase shift (MIPS) value to reflect the tissue’s condition. The hemorrhage model was established "
        "through the injection of autologous blood into the internal capsule of a rabbit’s brain. Ischemia was induced in the "
        "brain of a rabbit by bilateral carotid artery occlusion. Two types of animal models were measured with our device.";

    const char *abstract_results =
        "Results: The MIPS value gradually decreased with increasing injected blood and increased with ischemia time. "
        "The MIPS changes induced by the two types of strokes were exact opposites, and the absolute values of MIPS variation "
        "in the hemorrhagic and the ischemic groups were significantly larger than those of the normal control group (P < 0.05).";

    const char *abstract_conclusions =
        "Conclusions: The tested technique can differentiate ischemic stroke from hemorrhagic stroke on rabbit brain in a non-invasive, "
        "continuous, and bulk monitoring manner by using a simple and inexpensive apparatus.";

    const char *keywords =
        "Key words: Magnetic inductive phase shift (MIPS), Hemorrhagic stroke, Ischemic stroke";

    const char *email = "*E-mail: qminxinq@qq.com";

    const char *footer =
        "Preview only. Original open access article: doi:10.1186/s12938-017-0354-7";

    // Layout metrics
    const float margin = 32.0f;
    const float content_w = PAGE_W - margin * 2;
    const float line_h = 18.0f;

    while (!glfwWindowShouldClose(window)) {
        int win_w, win_h;
        glfwGetFramebufferSize(window, &win_w, &win_h);
        glViewport(0, 0, win_w, win_h);

        glClearColor(0.97f, 0.97f, 0.97f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        begin_ortho(PAGE_W, PAGE_H);

        // Header bar
        draw_rect(0, 0, PAGE_W, 64, 0.90f, 0.90f, 0.92f);
        draw_text(margin, 22, "Scientific Article Preview", 0.15f, 0.15f, 0.18f);

        // Title block
        float y = 96.0f;
        draw_text(margin, y, title, 0.0f, 0.0f, 0.0f);
        y += line_h * 2;

        // Authors and affiliation
        draw_text(margin, y, authors, 0.10f, 0.10f, 0.10f);
        y += line_h;
        draw_text(margin, y, affiliation, 0.10f, 0.10f, 0.10f);
        y += line_h * 2;

        // Abstract panel
        float panel_y = y - 8;
        float panel_h = 380.0f;
        draw_rect(margin - 8, panel_y, content_w + 16, panel_h, 1.0f, 1.0f, 1.0f);
        draw_rect(margin - 8, panel_y, content_w + 16, panel_h, 0.85f, 0.85f, 0.88f); // subtle border via overlay
        draw_text(margin, y, abstract_hdr, 0.0f, 0.0f, 0.0f);
        y += line_h * 1.2f;

        draw_paragraph(margin, y, content_w, line_h, abstract_bg, 0.05f, 0.05f, 0.05f);
        // Advance by estimated lines
        int lines_bg = (int)ceil((strlen(abstract_bg) / (content_w / CHAR_W)));
        y += lines_bg * line_h + line_h * 0.8f;

        draw_paragraph(margin, y, content_w, line_h, abstract_methods, 0.05f, 0.05f, 0.05f);
        int lines_methods = (int)ceil((strlen(abstract_methods) / (content_w / CHAR_W)));
        y += lines_methods * line_h + line_h * 0.8f;

        draw_paragraph(margin, y, content_w, line_h, abstract_results, 0.05f, 0.05f, 0.05f);
        int lines_results = (int)ceil((strlen(abstract_results) / (content_w / CHAR_W)));
        y += lines_results * line_h + line_h * 0.8f;

        draw_paragraph(margin, y, content_w, line_h, abstract_conclusions, 0.05f, 0.05f, 0.05f);
        int lines_concl = (int)ceil((strlen(abstract_conclusions) / (content_w / CHAR_W)));
        y += lines_concl * line_h + line_h * 1.2f;

        // Keywords and email
        draw_text(margin, y, keywords, 0.0f, 0.0f, 0.0f);
        y += line_h * 1.2f;
        draw_text(margin, y, email, 0.0f, 0.0f, 0.0f);
        y += line_h * 2.0f;

        // Footer
        draw_text(margin, PAGE_H - margin, footer, 0.25f, 0.25f, 0.28f);

        end_ortho();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
