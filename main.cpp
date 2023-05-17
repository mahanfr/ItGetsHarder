#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include "src/platformer.h"

#ifdef WIN32
#include <windows.h>
#endif

#define WINDOW_WIDTH 800
#define WINDOW_HIGHT 600

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Window *window = SDL_CreateWindow("Brahlers", 100, 100, WINDOW_WIDTH, WINDOW_HIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    platformer(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
