#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <sstream>
#include <string>
#include "Engine.h"
#include "Player.h"

using namespace std;

#ifdef _WIN32
#define MUS_PATH ".././assets/jump.wav"
#define SPRITE_PATH ".././assets/Old-hero-red.png";
#define FONT_PATH ".././assets/Oleaguid.ttf"
#else
#define MUS_PATH "./assets/jump.wav"
#define SPRITE_PATH "./assets/Old-hero.png"
#define FONT_PATH "./assets/Oleaguid.ttf"
#endif

Player::Player() {}

Player::Player(SDL_Renderer *renderer)
{
    size.x = 56;
    size.y = 64;
    char sprite[] = SPRITE_PATH;
    image = IMG_Load(sprite);
    texture = SDL_CreateTextureFromSurface(renderer, image);
    jumpingSound = Mix_LoadWAV(MUS_PATH);
}

SDL_Rect Player::hitbox()
{
    return SDL_Rect{pos.x, pos.y + 1, size.x, size.y};
}

SDL_Rect Player::feetHitbox()
{
    return SDL_Rect{pos.x, pos.y + size.y - 11, size.x, 12};
}

SDL_Rect Player::headHitbox()
{
    return SDL_Rect{pos.x, pos.y - 1, size.x, 12};
}

SDL_Rect Player::leftHitbox()
{
    return SDL_Rect{pos.x, pos.y, 12, size.y};
}

SDL_Rect Player::rightHitbox()
{
    return SDL_Rect{pos.x + size.x - 12, pos.y ,12 ,size.y};
}

int Player::get_player_collition() {
    for (Entity* entity : scene->objects)
    {
        if (entity->is_colliding(feetHitbox())) {return 3;}
        if (entity->is_colliding(leftHitbox())) {return 4;}
        if (entity->is_colliding(headHitbox())) {return 1;}
        if (entity->is_colliding(rightHitbox())) {return 2;}
    }
    return 0;
}

void Player::move(float x, float y)
{
    if (x < 0)
        playerDir = -1;
    else if (x > 0)
        playerDir = +1;
   
    for(int i=0; i < (int) abs(x); i++){
        int collition = get_player_collition();
        // if(collition == 3) break;
        this->pos.x += x;
    }

    for(int i=0; i < (int) abs(y); i++){
        int collition = get_player_collition();
        if(collition == 3){
            is_grounded = true;
            break;
        }else {
            is_grounded = false;
        }
        this->pos.y += y;
    }
}

void Player::playJumpSound()
{
    Mix_PlayChannel(-1, jumpingSound, 0);
}

void Player::update(SDL_Renderer *renderer)
{
    if(!is_grounded)
        move(0, 1);
    Uint32 ticks = SDL_GetTicks();
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A])
    {
        move(-1, 0);
        is_player_moving = true;
    }
    else if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D])
    {
        move(1, 0);
        is_player_moving = true;
    }
    else
        is_player_moving = false;

    if ((keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_SPACE]) && !is_jumping && is_grounded)
    {
        is_jumping = true;
    }

    if (keystates[SDL_SCANCODE_X] && !is_punching && !punching_cooldown)
    {
        is_punching = true;
        punching_duration = ticks;
        punching_cooldown = true;
    }
    else if (!keystates[SDL_SCANCODE_X])
    {
        punching_cooldown = false;
    }

    if (is_punching)
    {
        if (ticks - punching_duration >= 200)
        {
            is_punching = false;
            punching_duration = 0;
        }
        int sp_pos = 34;
        SDL_Rect srcrect = {sp_pos, 81, 14, 16};
        SDL_Rect dstrect = {pos.x, pos.y, 56, 64};
        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
        return;
    }

    if (is_jumping)
    {
        if (playerDistFromGround == 1)
            playJumpSound();

        if (playerDistFromGround <= jumpHight)
        {
            move(0,-5);
            playerDistFromGround++;
            int sp_pos = 17;
            SDL_Rect srcrect = {sp_pos, 48, 14, 16};
            SDL_Rect dstrect = {pos.x, pos.y, 56, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            return;
        }
        else
            is_jumping = false;
    }
    else
    {
        if (is_grounded)
        {
            playerDistFromGround = 0;
            is_jumping = false;
        }
        else
        {
            int sp_pos = 35;
            SDL_Rect srcrect = {sp_pos, 48, 14, 16};
            SDL_Rect dstrect = {pos.x, pos.y, 56, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            return;
        }
    }

    if (is_player_moving)
    {
        Uint32 sprite = (ticks / 100) % 6;
        int sp_pos = (sprite * 16) + 19;
        SDL_Rect srcrect = {sp_pos, 32, 13, 16};
        SDL_Rect dstrect = {pos.x, pos.y, 52, 64};
        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
    }
    else
    {
        Uint32 sprite = (ticks / 300) % 4;
        int sp_pos = (sprite * 16) + 19;
        SDL_Rect srcrect = {sp_pos, 16, 13, 16};
        SDL_Rect dstrect = {pos.x, pos.y, 52, 64};
        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
    }
}

void Player::destroy()
{
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    Mix_FreeChunk(jumpingSound);
    Mix_CloseAudio();
}
