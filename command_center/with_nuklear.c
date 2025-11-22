
#include <SDL2/SDL.h>
#define NK_IMPLEMENTATION
#include "nuklear.h"
#define NK_SDL_RENDERER_IMPLEMENTATION
#include "nuklear_sdl_renderer.h"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("Command Bar",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        800, 600, SDL_WINDOW_SHOWN);

    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    struct nk_context *ctx = nk_sdl_init(win, renderer);

    struct nk_color background = nk_rgb(30,30,30);

    char cmd[256] = {0};
    int running = 1;

    while (running) {
        SDL_Event evt;
        while (SDL_PollEvent(&evt)) {
            if (evt.type == SDL_QUIT) running = 0;
            nk_sdl_handle_event(&evt);
        }

        nk_sdl_new_frame(ctx);

        /* GUI */
        if (nk_begin(ctx, "MainWindow", nk_rect(0,0,800,600),
                NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_BACKGROUND)) {

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_edit_string_zero_terminated(ctx, NK_EDIT_FIELD, cmd, sizeof(cmd), nk_filter_default);

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Run Command")) {
                printf("User command: %s\n", cmd);
                // execute your logic
            }
        }
        nk_end(ctx);

        /* Render */
        SDL_SetRenderDrawColor(renderer, background.r, background.g, background.b, 255);
        SDL_RenderClear(renderer);
        nk_sdl_render(NK_ANTI_ALIASING_ON);
        SDL_RenderPresent(renderer);
    }

    nk_sdl_shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
