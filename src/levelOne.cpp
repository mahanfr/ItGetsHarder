#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <complex>
#include <cstring>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include "levelOne.h"
#include "utils.h"
#include "Engine.h"
#include "Player.h"

using namespace std;

#ifdef _WIN32
#define BACKGROUND_IMAGE_PATH ".././assets/level-one/background.png"
#define FORGROUND_IMAGE_PATH ".././assets/level-one/forground.png"
#define COLUD_ONE_IMAGE_PATH ".././assets/level-one/cloud-one.png"
#define CLOUD_TWO_IMAGE_PATH ".././assets/level-one/cloud-two.png"
#else
#define BACKGROUND_IMAGE_PATH "./assets/level-one/background.png"
#define FORGROUND_IMAGE_PATH "./assets/level-one/forground.png"
#define CLOUD_ONE_IMAGE_PATH "./assets/level-one/cloud-one.png"
#define CLOUD_TWO_IMAGE_PATH "./assets/level-one/cloud-two.png"
#endif

typedef struct Vec2 {
    float x;
    float y;
} Vec2;


class Platform : public Entity {
public: 
    Platform(SDL_Point pos){ 
        this-> pos = pos;
        this->size = {WINDOW_WIDTH, 80};
        this->is_static = false;
    }
    
    void update(SDL_Renderer * renderer) override {
        SDL_Rect srcrect = {pos.x, pos.y, size.x, size.y};
        SDL_SetRenderDrawColor(renderer,0,0,0,255);
        SDL_RenderFillRect(renderer, &srcrect);
    }
};

class BackgroudLeyer : public Entity{
private:
    SDL_Surface* image;
    SDL_Texture* texture;
public:
    BackgroudLeyer(string path) {
        image = IMG_Load(path.c_str());
    }

    void start(SDL_Renderer *renderer) override {
        texture = SDL_CreateTextureFromSurface(renderer, image);
    }

    void update(SDL_Renderer *renderer) override {
        SDL_Rect srcrect = {0, 0, image->w, image->h};
        SDL_Rect dstrect = {0, 0, WINDOW_WIDTH, WINDOW_HIGHT};
        SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
    }
    void destroy() override {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
    }
};

class Cloud : public Entity {
private:
    SDL_Surface* image;
    SDL_Texture* texture;
public:
    Cloud(string path) {
        image = IMG_Load(path.c_str());
        pos.x = rand() % WINDOW_WIDTH;
        pos.y = rand() % 200;
    }

    void start(SDL_Renderer *renderer) override {
        texture = SDL_CreateTextureFromSurface(renderer, image);
    }

    void update(SDL_Renderer* renderer) override {
        SDL_Rect srcrect = {0, 0, image->w, image->h};
        SDL_Rect dstrect = {pos.x, pos.y, image->w, image->h};
        SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
        pos.x -= 1;
        if(pos.x < -image->w) {
            pos.y = rand() % 200;
            pos.x = image->w + WINDOW_WIDTH;
        }
    }

    void destroy() override{
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
    }
};

void Scene1::start(SDL_Renderer* renderer) {
    objects.push_back(new BackgroudLeyer(BACKGROUND_IMAGE_PATH));
    objects.push_back(new Cloud(CLOUD_ONE_IMAGE_PATH));
    objects.push_back(new Cloud(CLOUD_TWO_IMAGE_PATH));
    objects.push_back(new BackgroudLeyer(FORGROUND_IMAGE_PATH));
    objects.push_back(new Platform(SDL_Point{0,WINDOW_HIGHT/2+150}));
    objects.push_back(new Player(renderer));
    objects[0]->start(renderer);
    objects[1]->start(renderer);
    objects[2]->start(renderer);
    objects[3]->start(renderer);

    objects[5]->scene = this;
}

void Scene1::update(void) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    for(Entity* entity : objects) {
        entity->update(renderer);
    }
}

void Scene1::destroy() {
    for(Entity* entity : objects) {
        entity->destroy();
    }
    objects.clear();
}
