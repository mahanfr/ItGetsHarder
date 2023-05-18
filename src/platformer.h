#pragma once
#include "utils.h"
#include <SDL2/SDL_render.h>

class Player;
typedef struct TestLevelState {
    SDL_Renderer* renderer; 
    Player* player;
} TestLevelState;

TestLevelState GetTestStateLevel(SDL_Renderer* renderer);

void UpdateTestLevel(TestLevelState state);

void DestroyTestLevel(TestLevelState state);
