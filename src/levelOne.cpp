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
    vector<Entity *> objects;

    Scene(SDL_Renderer* renderer) {
        this -> renderer = renderer;
    }

    void start_objects(SDL_Renderer* renderer) {
        objects[0]->start(renderer);
        objects[1]->start(renderer);
        objects[2]->start(renderer);
        objects[3]->start(renderer);

        objects[5]->scene = this;
    }

    void update(void) {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_RenderClear(renderer);
        for(Entity* entity : objects) {
            entity->update(renderer);
        }
    }

    void destroy() {
        for(Entity* entity : objects) {
            entity->destroy();
        }
        objects.clear();
    }
};

class Platform : public Entity {
public: 
    Platform(SDL_Point pos){ 
        this-> pos = pos;
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


class Player: public Entity {
public:
    bool start = true;
    bool is_static = false;
    float mass = 5;
    float gravity = 10;
    Vec2 force = {0,0};
    Vec2 velocity = {0,0};
    Uint32 lastUpdate = 0;
    float fallingTime = 0;

    Player(){
        this->pos = {(WINDOW_WIDTH/2), 0};
        this->size = {40, 80};
    }

    SDL_Rect hitbox()
    {
        return SDL_Rect{pos.x, pos.y + 1, size.x, size.y};
    }

    SDL_Rect feetHitbox()
    {
        return SDL_Rect{pos.x, pos.y + size.y - 11, size.x, 12};
    }

    SDL_Rect headHitbox()
    {
        return SDL_Rect{pos.x, pos.y - 1, size.x, 12};
    }

    SDL_Rect leftHitbox()
    {
        return SDL_Rect{pos.x, pos.y, 12, size.y};
    }

    SDL_Rect rightHitbox()
    {
        return SDL_Rect{pos.x + size.x - 12, pos.y ,12 ,size.y};
    }

    /*
     * 0 = no_collide , 1 = head , 2 = right
     * 3 = feet , 4 = left
     * */
    int get_player_collition() {
        
        for (Entity* entity : scene->objects)
        {
            if (entity->is_colliding(feetHitbox())) return 3;
            if (entity->is_colliding(leftHitbox())) return 4;
            if (entity->is_colliding(headHitbox())) return 1;
            if (entity->is_colliding(rightHitbox())) return 2;
        }
        return 0;
    }

    void move(float x = 0,float y = 0) {
        for(int i = (int) y; i > 0; --i){
            int collition = get_player_collition();
            if(collition == 3) break;
            pos.y += 1;
        }
        for(int i = (int) x; i > 0; --i){
            int collition = get_player_collition();
            if(collition == 3) break;
            calculate_velocity();
            pos.x += 1;
        }
    }

    void calculate_velocity() {
        int collition = get_player_collition();
        if(collition == 3){
            velocity.y = 0;
            return;
        }
        velocity.y = mass * gravity;
    }

    void apply_velocity() {
        auto current = SDL_GetTicks();
        fallingTime += (current - lastUpdate) / 1000.f;
        float dt = (current - lastUpdate) / 1000.f;
        // Cap Timer to never be more than 1 sec
        if (start){
            dt = 0.1;
            fallingTime = 0;
            start = false;
        }
        move(0,velocity.y * fallingTime * dt * 99.4f);
        lastUpdate = current;
    }

    void update(SDL_Renderer * renderer) override {
        calculate_velocity();
        apply_velocity();
        SDL_Rect srcrect = {(int) pos.x, (int) pos.y, (int)size.x, (int)size.y};
        SDL_SetRenderDrawColor(renderer,255,0,0,255);
        SDL_RenderFillRect(renderer, &srcrect);
    }

};

LevelOneState GetLevelOneState(SDL_Renderer* renderer) {
    auto scene = new Scene(renderer);
    scene->objects.push_back(new BackgroudLeyer(BACKGROUND_IMAGE_PATH));
    scene->objects.push_back(new Cloud(CLOUD_ONE_IMAGE_PATH));
    scene->objects.push_back(new Cloud(CLOUD_TWO_IMAGE_PATH));
    scene->objects.push_back(new BackgroudLeyer(FORGROUND_IMAGE_PATH));
    scene->objects.push_back(new Platform(SDL_Point{WINDOW_WIDTH/2-20,WINDOW_HIGHT/2+150}));
    scene->objects.push_back(new Player());
    scene->start_objects(renderer);
    auto state = LevelOneState{scene};
    return state;
}

void UpdateLevelOne(LevelOneState state){
    state.scene->update();
} 

void DestroyLevelOne(LevelOneState state){
    state.scene->destroy();
}

