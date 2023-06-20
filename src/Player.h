#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <sstream>
#include <string>
#include "Engine.h"

#ifndef INCLUDE_PLAYER_H
#define INCLUDE_PLAYER_H
class Player: public Entity
{
private:
    SDL_Surface *image;
    SDL_Texture *texture;
    int jumpHight = 15;
    bool is_grounded = false;
    int playerDistFromGround = 0;
    bool is_jumping = false;
    bool is_punching = false;
    bool punching_cooldown = 0;
    Uint32 punching_duration = 0;
    int punch_afp = 0;
    int speed = 4;

public:
    bool is_player_moving = false;
    int playerDir = +1;
    Mix_Chunk *jumpingSound;
    float health = 100;

    Player();

    Player(SDL_Renderer *renderer);
    
    int get_player_collition();
    SDL_Rect hitbox();
    SDL_Rect feetHitbox();
    SDL_Rect headHitbox();
    SDL_Rect leftHitbox();
    SDL_Rect rightHitbox();

    void move(float x, float y);

    void destroy();
    void playJumpSound();
    void update(SDL_Renderer *renderer);
};
#endif


// class Player: public Entity {
// public:
//     bool start = true;
//     bool is_static = false;
//     float mass = 5;
//     float gravity = 50;
//     Vec2 force = {0,0};
//     Vec2 velocity = {0,0};
//     Uint32 lastUpdate = 0;
//     float fallingTime = 0;
// 
//     Player(){
//         this->pos = {(WINDOW_WIDTH/2), 0};
//         this->size = {40, 80};
//     }
// 
//     SDL_Rect hitbox()
//     {
//         return SDL_Rect{pos.x, pos.y + 1, size.x, size.y};
//     }
// 
//     SDL_Rect feetHitbox()
//     {
//         return SDL_Rect{pos.x, pos.y + size.y - 11, size.x, 12};
//     }
// 
//     SDL_Rect headHitbox()
//     {
//         return SDL_Rect{pos.x, pos.y - 1, size.x, 12};
//     }
// 
//     SDL_Rect leftHitbox()
//     {
//         return SDL_Rect{pos.x, pos.y, 12, size.y};
//     }
// 
//     SDL_Rect rightHitbox()
//     {
//         return SDL_Rect{pos.x + size.x - 12, pos.y ,12 ,size.y};
//     }
// 
//     /*
//      * 0 = no_collide , 1 = head , 2 = right
//      * 3 = feet , 4 = left
//      * */
//     int get_player_collition() {
//         
//         for (Entity* entity : scene->objects)
//         {
//             if (entity->is_colliding(feetHitbox())) {return 3;}
//             if (entity->is_colliding(leftHitbox())) {return 4;}
//             if (entity->is_colliding(headHitbox())) {return 1;}
//             if (entity->is_colliding(rightHitbox())) {return 2;}
//         }
//         return 0;
//     }
// 
//     void move(float x = 0,float y = 0) {
//         for(int i = (int) y; i > 0; --i){
//             int collition = get_player_collition();
//             if(collition == 3) break;
//             pos.y += 1;
//         }
//         for(int i = (int) x; i > 0; --i){
//             int collition = get_player_collition();
//             if(collition == 3) break;
//             calculate_velocity();
//             pos.x += 1;
//         }
//     }
// 
//     void calculate_velocity() {
//         int collition = get_player_collition();
//         if(collition == 3){
//             velocity.y = 0;
//             return;
//         }
//         velocity.y = mass * gravity;
//     }
// 
//     void apply_velocity() {
//         auto current = SDL_GetTicks();
//         fallingTime += (current - lastUpdate) / 1000.f;
//         float dt = (current - lastUpdate) / 1000.f;
//         // Cap Timer to never be more than 1 sec
//         if (start){
//             dt = 0.1;
//             fallingTime = 0;
//             start = false;
//         }
//         move(0,velocity.y * fallingTime * dt * 2.4f);
//         lastUpdate = current;
//     }
// 
//     void update(SDL_Renderer * renderer) override {
//         calculate_velocity();
//         apply_velocity();
//         SDL_Rect srcrect = {(int) pos.x, (int) pos.y, (int)size.x, (int)size.y};
//         SDL_SetRenderDrawColor(renderer,255,0,0,255);
//         SDL_RenderFillRect(renderer, &srcrect);
//     }
// 
// };
