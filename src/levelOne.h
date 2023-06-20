#pragma once
#include <SDL2/SDL.h>
#include "Engine.h"

class Scene1 : public Scene {
    public:
    Scene1(SDL_Renderer *renderer): Scene(renderer) {} 

    void start(SDL_Renderer* renderer);

    void update(void);

    void destroy();
};
