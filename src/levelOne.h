#pragma once
#include <SDL2/SDL.h>

class Scene;

typedef struct LevelOneState {
    Scene* scene;
} LevelOneState;

LevelOneState GetLevelOneState(SDL_Renderer* renderer);

void UpdateLevelOne(LevelOneState state);

void DestroyLevelOne(LevelOneState state);
