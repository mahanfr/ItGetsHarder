#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "src/platformer.h"

#define WINDOW_WIDTH 800
#define WINDOW_HIGHT 600

int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("ItGetsHarder", 100, 100, WINDOW_WIDTH, WINDOW_HIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
    
    platformer(renderer);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}