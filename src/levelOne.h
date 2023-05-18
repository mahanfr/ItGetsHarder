#pragma once
#include <SDL2/SDL.h>

typedef struct LevelOneState {
    SDL_Renderer* renderer; 
} LevelOneState;

LevelOneState GetLevelOneState(SDL_Renderer* renderer);

void UpdateLevelOne(LevelOneState state);

void DestroyLevelOne(LevelOneState state);
