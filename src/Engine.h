#pragma once
#include <SDL2/SDL.h>
#include <cstdlib>
#include <string>
#include <vector>
#ifndef _INCLUDE_ENGINE_H
#define _INCLUDE_ENGINE_H

class Scene;

class Entity {
public:
    SDL_Point pos = {0,0};
    SDL_Point size = {100,100};
    bool is_static = true;
    long eid = rand();
    std::string tag = "";
    Scene* scene;
    bool editMode = false;

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

    virtual void addObject(Entity *ent) {
        objects.push_back(ent);
        ent->start(renderer);
    }

    virtual void addObject(Entity *ent, Scene *scn) {
        objects.push_back(ent);
        ent->scene = scn;
    }
    
    virtual void update(void){}

    virtual void destroy() {} 
};

class Editor {
public:
    SDL_Renderer* renderer;
    Scene *scene = nullptr;
    bool drag = false;
    long drag_eid;
    SDL_Point drag_offset = {0,0};

    Editor(SDL_Renderer* renderer);
    Editor(Scene* scene);
    bool is_mouseOnEntity(int mouse_x, int mouse_y, Entity *entity);
    void setScene(Scene* scene);
    void drawWireFrame(Entity* entity);
    void update();
};

#endif
