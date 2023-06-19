#pragma once
#include <SDL2/SDL.h>
#include <vector>
#ifndef _INCLUDE_ENGINE_H
#define _INCLUDE_ENGINE_H

class Scene;

class Entity {
public:
    SDL_Point pos = {0,0};
    SDL_Point size = {100,100};
    bool is_static = true;
    Scene* scene;

    Entity() {}

    bool is_colliding(SDL_Rect src)
    {
        SDL_Rect hb{pos.x, pos.y, size.x, size.y};
        if(!is_static) {
            return (SDL_HasIntersection(&hb, &src)); 
        }
        return false;
    }
    virtual void start(SDL_Renderer * renderer) {}
    virtual void start() {}
    virtual void update(SDL_Renderer * renderer) {}
    virtual void update() {}
    virtual void destroy() {}
};

class Scene {
    public:
    SDL_Renderer* renderer;
    std::vector<Entity *> objects;
    
    Scene(SDL_Renderer* renderer) {
        this -> renderer = renderer;
    }
    
    virtual void start() {}

    virtual void update(void){}

    virtual void destroy() {} 
};

#endif
