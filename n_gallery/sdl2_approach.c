/*  gcc gallery.c tinyfiledialogs.c -lSDL2 -std=c11  */
#include "tinyfiledialogs.h"
#include <SDL2/SDL.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(void)
{
    /* 1. let the user pick one picture  */
    const char *filters[] = {"*.jpg","*.png","*.bmp"};
    const char *file = tinyfd_openFileDialog(
                           "Choose an image","",
                           3,filters,"Image files",0);
    if(!file) return 0;

    /* 2. load pixels  */
    int w,h,n;
    unsigned char *pixels = stbi_load(file,&w,&h,&n,4);
    if(!pixels){ tinyfd_messageBox("Error","Load failed","ok","error",0); return 1;}

    /* 3. create window and show it  */
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window   *win = SDL_CreateWindow("Gallery",SDL_WINDOWPOS_CENTERED,
                                         SDL_WINDOWPOS_CENTERED,w,h,0);
    SDL_Renderer *ren = SDL_CreateRenderer(win,-1,SDL_RENDERER_ACCELERATED);
    SDL_Texture  *tex = SDL_CreateTexture(ren,SDL_PIXELFORMAT_RGBA8888,
                                          SDL_TEXTUREACCESS_STATIC,w,h);
    SDL_UpdateTexture(tex,NULL,pixels,w*4);
    SDL_RenderClear(ren);
    SDL_RenderCopy(ren,tex,NULL,NULL);
    SDL_RenderPresent(ren);
    SDL_Delay(4000);            /* simple pause so you can see it */

    /* 4. clean-up  */
    stbi_image_free(pixels);
    SDL_DestroyTexture(tex);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
