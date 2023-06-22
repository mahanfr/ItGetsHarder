#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cstdio>
#include <sstream>
#include <string>
#include "Engine.h"
#include "Player.h"

using namespace std;


Player::Player() {}
Player::Player(SDL_Renderer *renderer) {
    size.x = 56;
    size.y = 64;
    is_static = false;
    tag = "player";
    char sprite[] = SPRITE_PATH;
    image = IMG_Load(sprite);
    texture = SDL_CreateTextureFromSurface(renderer, image);
    jumpingSound = Mix_LoadWAV(MUS_PATH);
    if (jumpingSound == NULL) {
        fprintf(stderr, "Error: Sound can not be played\n");
    }
}

SDL_Rect Player::hitbox() {
    return SDL_Rect{pos.x, pos.y + 1, size.x, size.y};
}
SDL_Rect Player::feetHitbox() {
    return SDL_Rect{pos.x + 8, pos.y + size.y - 1, size.x - 16, 1};
}
SDL_Rect Player::headHitbox() {
    return SDL_Rect{pos.x + 8, pos.y - 1, size.x - 16, 1};
}
SDL_Rect Player::leftHitbox() {
    return SDL_Rect{pos.x ,pos.y + 12, 1, size.y - 24};
}
SDL_Rect Player::rightHitbox() {
    return SDL_Rect{pos.x + size.x - 1, pos.y + 12 ,1 ,size.y - 24};
}

int Player::get_player_collition() {
    int collition = 0;
    for (Entity* entity : scene->objects)
    {
        if (entity->tag == "player") {continue;}
        if (entity->is_colliding(feetHitbox()))  {
            collition = collition | PLAYER_COL_FEET;
        }
        if (entity->is_colliding(leftHitbox()))  {
            collition = collition | PLAYER_COL_LEFT;
        }
        if (entity->is_colliding(rightHitbox())) {
            collition = collition | PLAYER_COL_RIGHT;
        }
        if (entity->is_colliding(headHitbox()))  {
            is_jumping = false;
            collition = collition | PLAYER_COL_HEAD;
        }
    }
    return collition;
}

void Player::move(float x, float y) {
    if (x < 0)
        playerDir = -1;
    else if (x > 0)
        playerDir = +1;
   
    for(int i=0; i < (int) abs(x); i++){
        int collition = get_player_collition();
        if((collition & PLAYER_COL_LEFT) != 0 && x < 0) {
            break;
        }
        if((collition & PLAYER_COL_RIGHT) != 0 && x > 0) {
            break;
        }
        this->pos.x += x * speed;
    }

    for(int i=0; i < (int) abs(y); i++){
        int collition = get_player_collition();
        if((collition & PLAYER_COL_FEET) != 0 && y > 0){
            is_grounded = true;
            break;
        }else {
            is_grounded = false;
        }

        if ((collition & PLAYER_COL_HEAD) && y < 0) {
            is_jumping = false;
            break;
        }
        this->pos.y += y * speed * 1.2;
    }
}

void Player::playJumpSound()
{
    auto err = Mix_PlayChannel(-1, jumpingSound, 0);
    if (err == -1){
        printf("Warning: Can not Play the sound! code: %d\n",err);
    }
}

void Player::update(SDL_Renderer *renderer)
{
    move(0,1);
    Uint32 ticks = SDL_GetTicks();
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) {
        move(-1, 0);
        is_player_moving = true;
    }
    else if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) {
        move(1, 0);
        is_player_moving = true;
    } else
        is_player_moving = false;

    if ((keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W] 
            || keystates[SDL_SCANCODE_SPACE]) && !is_jumping && is_grounded) {
        is_jumping = true;
    }

    if (is_jumping) {
        if (playerDistFromGround == 1)
            playJumpSound();
         
        if (playerDistFromGround <= jumpHight) {
            move(0,-1.9);
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
    else {
        if (is_grounded) {
            playerDistFromGround = 0;
            is_jumping = false;
        }
        else {
            int sp_pos = 35;
            SDL_Rect srcrect = {sp_pos, 48, 14, 16};
            SDL_Rect dstrect = {pos.x, pos.y, 56, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            return;
        }
    }

    if (is_player_moving) {
        Uint32 sprite = (ticks / 100) % 6;
        int sp_pos = (sprite * 16) + 19;
        SDL_Rect srcrect = {sp_pos, 32, 13, 16};
        SDL_Rect dstrect = {pos.x, pos.y, 52, 64};
        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
    }
    else {
        Uint32 sprite = (ticks / 300) % 4;
        int sp_pos = (sprite * 16) + 19;
        SDL_Rect srcrect = {sp_pos, 16, 13, 16};
        SDL_Rect dstrect = {pos.x, pos.y, 52, 64};
        SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
    }
}

void Player::destroy() {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(image);
    Mix_FreeChunk(jumpingSound);
    Mix_CloseAudio();
}
