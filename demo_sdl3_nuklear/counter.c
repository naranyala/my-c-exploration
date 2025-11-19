
#include <stdio.h>
#include <SDL2/SDL.h>
#include <GL/glew.h>
#include "nuklear.h"
#include "nuklear_sdl_gl3.h"

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

int main(void) {
    SDL_Window *win;
    SDL_GLContext glContext;
    struct nk_context *ctx;
    int counter = 0;

    /* SDL init */
    SDL_Init(SDL_INIT_VIDEO);
    win = SDL_CreateWindow("Nuklear Counter Demo",
                           SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                           WINDOW_WIDTH, WINDOW_HEIGHT,
                           SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN);
    glContext = SDL_GL_CreateContext(win);
    glewInit();

    /* Nuklear init */
    ctx = nk_sdl_init(win);

    /* Load default font */
    struct nk_font_atlas *atlas;
    nk_sdl_font_stash_begin(&atlas);
    nk_sdl_font_stash_end();

    /* Main loop */
    int running = 1;
    while (running) {
        SDL_Event evt;
        nk_input_begin(ctx);
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) running = 0;
            nk_sdl_handle_event(&evt);
        }
        nk_input_end(ctx);

        /* Nuklear UI */
        if (nk_begin(ctx, "Counter Window",
                     nk_rect(50, 50, 220, 150),
                     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE)) {

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_label(ctx, "Click the button:", NK_TEXT_LEFT);

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_button_label(ctx, "Increment")) {
                counter++;
            }
            nk_labelf(ctx, NK_TEXT_LEFT, "Count: %d", counter);
        }
        nk_end(ctx);

        /* Render */
        glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT);
        nk_sdl_render(NK_ANTI_ALIASING_ON);
        SDL_GL_SwapWindow(win);
    }

    nk_sdl_shutdown();
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
