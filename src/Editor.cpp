#include <SDL2/SDL.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <cstdio>
#include <cstdlib>
#include "Engine.h"

Editor::Editor(SDL_Renderer* renderer) {
    this->renderer = renderer;
}

Editor::Editor(Scene *scene) {
    this->renderer = scene->renderer;
    this->scene = scene;
}

void Editor::setScene(Scene* scene) {
    this->scene = scene;
}

void Editor::drawWireFrame(Entity *entity) {
    SDL_Rect rect = SDL_Rect{entity->pos.x,entity->pos.y,entity->size.x,entity->size.y};
    SDL_SetRenderDrawColor(renderer, 168, 230, 255, 255);
    SDL_RenderFillRect(renderer, &rect);
}

bool Editor::is_mouseOnEntity(int mouse_x, int mouse_y, Entity *entity) {
    return (mouse_x <= entity->pos.x + entity->size.x && mouse_x >= entity->pos.x)
        &&(mouse_y <= entity->pos.y + entity->size.y && mouse_y >= entity->pos.y);
}

void Editor::update() {
    if (scene == nullptr) {
        fprintf(stderr, "Error: Scene Should be Set Befor running rendering operations");
        exit(-1);
    }
    int mouse_x,mouse_y = 0;
    Uint32 state = SDL_GetMouseState(&mouse_x, &mouse_y);
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    for(auto* entity : scene->objects) {
        if(!entity->is_static) {
            if(state != 1) {
                drag = false;
                entity->editMode = false;
            }
            if (!drag && is_mouseOnEntity(mouse_x, mouse_y, entity)) {
                if(state == 1){
                    drag = true;
                    drag_eid = entity->eid;
                    drag_offset = {(entity->pos.x - mouse_x), (entity->pos.y - mouse_y)};
                    entity->editMode = true;
                }
            }

            if(drag && drag_eid == entity->eid) {
                entity->pos.x = drag_offset.x + mouse_x;
                entity->pos.y = drag_offset.y + mouse_y;
            }
            drawWireFrame(entity);
            entity->update(renderer);
        }else {
            entity->update(renderer);
        }
    }
}

