#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_BUTTON_TRIGGER_ON_RELEASE
#include "../vendor/Nuklear/nuklear.h"

#include <GL/gl.h>
#include <GLFW/glfw3.h>

int main() {
    GLFWwindow *win;
    struct nk_context *ctx;
    struct nk_colorf bg;

    /* GLFW setup */
    if (!glfwInit()) return -1;
    win = glfwCreateWindow(800, 600, "Nuklear Counter Demo", NULL, NULL);
    if (!win) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);

    /* Nuklear setup */
    ctx = nk_glfw3_init(win, NK_GLFW3_INSTALL_CALLBACKS);

    /* GUI state */
    int counter = 0;

    /* Main loop */
    while (!glfwWindowShouldClose(win)) {
        glfwPollEvents();

        /* Nuklear frame */
        nk_glfw3_new_frame();

        /* GUI */
        if (nk_begin(ctx, "Counter Demo", nk_rect(50, 50, 200, 100),
            NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE)) {
            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "Counter:", NK_TEXT_LEFT);
            nk_label(ctx, "%d", NK_TEXT_LEFT, counter);
            if (nk_button_label(ctx, "Increment")) {
                counter++;
            }
        }
        nk_end(ctx);

        /* Render */
        glfwGetWindowSize(win, (int*)&width, (int*)&height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        nk_glfw3_render(NK_ANTI_ALIASING_ON);
        glfwSwapBuffers(win);
    }

    /* Cleanup */
    nk_glfw3_shutdown();
    glfwTerminate();
    return 0;
}
