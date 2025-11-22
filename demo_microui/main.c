
#include <SDL2/SDL.h>
#include "../vendor/microui/src/microui.h"

#define WIDTH 800
#define HEIGHT 600

// Global microui context
mu_Context ctx;

// Simple renderer: draws rectangles and text
void render(mu_Context *ctx, SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
        switch (cmd->type) {
        case MU_COMMAND_TEXT:
            // TODO: add text rendering with SDL_ttf
            break;
        case MU_COMMAND_RECT: {
            SDL_Rect r = {
                cmd->rect.rect.x,
                cmd->rect.rect.y,
                cmd->rect.rect.w,
                cmd->rect.rect.h
            };
            SDL_SetRenderDrawColor(renderer,
                                   cmd->rect.color.r,
                                   cmd->rect.color.g,
                                   cmd->rect.color.b,
                                   cmd->rect.color.a);
            SDL_RenderFillRect(renderer, &r);
        } break;
        case MU_COMMAND_CLIP:
            // Not implemented in this demo
            break;
        }
    }

    SDL_RenderPresent(renderer);
}

void demo_window(mu_Context *ctx) {
    if (mu_begin_window(ctx, "Demo Window", mu_rect(50, 50, 300, 200))) {
        mu_layout_row(ctx, 2, (int[]){ -1, -1 }, 25);
        mu_label(ctx, "Hello, microui!");
        if (mu_button(ctx, "Click Me")) {
            printf("Button pressed!\n");
        }
        mu_end_window(ctx);
    }
}

int main(void) {
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *win = SDL_CreateWindow("microui demo",
                                       SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);

    mu_init(&ctx);

    int running = 1;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = 0;
            // Map SDL events to microui input
            if (e.type == SDL_MOUSEBUTTONDOWN) mu_input_mousedown(&ctx, e.button.x, e.button.y, MU_MOUSE_LEFT);
            if (e.type == SDL_MOUSEBUTTONUP)   mu_input_mouseup(&ctx, e.button.x, e.button.y, MU_MOUSE_LEFT);
            if (e.type == SDL_MOUSEMOTION)     mu_input_mousemove(&ctx, e.motion.x, e.motion.y);
        }

        mu_begin(&ctx);
        demo_window(&ctx);
        mu_end(&ctx);

        render(&ctx);
        SDL_Delay(16); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
