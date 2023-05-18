#include <SDL2/SDL.h>
#include "levelOne.h"

LevelOneState GetLevelOneState(SDL_Renderer* renderer) {
    return LevelOneState{renderer};
}

void UpdateLevelOne(LevelOneState state){
    SDL_SetRenderDrawColor(state.renderer, 230, 230, 0, 255);
    SDL_RenderClear(state.renderer);
}

void DestroyLevelOne(LevelOneState state){}
