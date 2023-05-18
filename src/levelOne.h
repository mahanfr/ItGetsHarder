#pragma once
#include <SDL2/SDL.h>

class Background;

typedef struct LevelOneState {
    SDL_Renderer* renderer;
    Background* background;
} LevelOneState;

LevelOneState GetLevelOneState(SDL_Renderer* renderer);

void UpdateLevelOne(LevelOneState state);

void DestroyLevelOne(LevelOneState state);
