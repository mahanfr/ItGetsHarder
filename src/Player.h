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
