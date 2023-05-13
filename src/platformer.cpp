#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "utils.h"
#include "platformer.h"

#ifdef _WIN32
#define MUS_PATH ".././assets/jump.wav"
#define SPRITE_PATH ".././assets/Old-hero.png";
#else
#define MUS_PATH "./assets/jump.wav"
#define SPRITE_PATH "./assets/Old-hero.png";
#endif
#define WINDOW_WIDTH 800
#define WINDOW_HIGHT 600

class Player
{
private:
    SDL_Surface *image;
    SDL_Texture *texture;
    int jumpHight = 15;
    int playerDistFromGround = 0;
    bool is_jumping = false;

public:
    int speed = 4;
    int x = 100; // make it private someday
    int y = 200;
    bool is_player_moving = false;
    bool is_grounded = false;
    int playerDir = +1;
    Mix_Chunk *jumpingSound;

    Player(SDL_Renderer *renderer, char *sprite)
    {
        image = IMG_Load(sprite);
        texture = SDL_CreateTextureFromSurface(renderer, image);
        jumpingSound = Mix_LoadWAV(MUS_PATH);
    }

    SDL_Rect hitbox()
    {
        return SDL_Rect{x, y + 1, 52, 64};
    }
    SDL_Rect feet_hitbox()
    {
        return SDL_Rect{x + 5, y + 52, 25, 12};
    }
    SDL_Rect head_hitbox()
    {
        return SDL_Rect{x + 5, y, 25, 12};
    }

    void move(int x, int y)
    {
        if (x < 0)
        {
            playerDir = -1;
        }
        else if (x > 0)
        {
            playerDir = +1;
        }
        this->x += x * speed * 1.5;
        this->y += y * speed * 1.5;
    }

    void destroy()
    {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
        Mix_FreeChunk(jumpingSound);
        Mix_CloseAudio();
    }

    void playJumpSound()
    {
        Mix_PlayChannel(-1, jumpingSound, 0);
    }

    void render(SDL_Renderer *renderer, Uint32 ticks)
    {
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_LEFT])
        {
            is_player_moving = true;
            move(-1, 0);
        }
        else if (keystates[SDL_SCANCODE_RIGHT])
        {
            is_player_moving = true;
            move(1, 0);
        }
        else
        {
            is_player_moving = false;
        }

        if (keystates[SDL_SCANCODE_UP] && !is_jumping && is_grounded)
        {
            is_jumping = true;
        }

        if (is_jumping)
        {
            if (playerDistFromGround == 1)
            {
                playJumpSound();
            }
            if (playerDistFromGround <= jumpHight)
            {
                move(0, -1);
                playerDistFromGround++;
                int sp_pos = 35;
                SDL_Rect srcrect = {sp_pos, 48, 14, 16};
                SDL_Rect dstrect = {x, y, 56, 64};
                SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
                return;
            }
            else
            {
                is_jumping = false;
            }
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
                move(0, 1);
                int sp_pos = true ? 17 : 35;
                SDL_Rect srcrect = {sp_pos, 48, 14, 16};
                SDL_Rect dstrect = {x, y, 56, 64};
                SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
                return;
            }
        }

        if (is_player_moving)
        {
            Uint32 sprite = (ticks / 100) % 6;
            int sp_pos = (sprite * 16) + 19;
            SDL_Rect srcrect = {sp_pos, 32, 13, 16};
            SDL_Rect dstrect = {x, y, 52, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
        }
        else
        {
            Uint32 sprite = (ticks / 300) % 4;
            int sp_pos = (sprite * 16) + 19;
            SDL_Rect srcrect = {sp_pos, 16, 13, 16};
            SDL_Rect dstrect = {x, y, 52, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
        }
    }
};

void platformer(SDL_Renderer *renderer)
{
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
    bool running = true;

    char sprite[] = SPRITE_PATH;
    Player player(renderer, sprite);
    while (running)
    {
        Uint32 ticks = SDL_GetTicks();
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if ((SDL_QUIT == ev.type) ||
                (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
            {
                running = false;
                break;
            }
        }

        // Render Block
        SDL_SetRenderDrawColor(renderer, 168, 230, 255, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_Rect ground{0, 300, 200, 50};
        SDL_RenderFillRect(renderer, &ground);

        SDL_SetRenderDrawColor(renderer, 255, 125, 70, 255);
        SDL_Rect ground1{50, 150, 200, 50};
        SDL_RenderFillRect(renderer, &ground1);

        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_Rect ground2{260, 360, 180, 50};
        SDL_RenderFillRect(renderer, &ground2);

        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_Rect ground3{500, 420, 400, 50};
        SDL_RenderFillRect(renderer, &ground3);

        SDL_Rect feetHitbos = player.feet_hitbox();
        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_RenderFillRect(renderer, &feetHitbos);

        SDL_Rect headHitbox = player.head_hitbox();
        SDL_SetRenderDrawColor(renderer, 255, 125, 70, 255);
        SDL_RenderFillRect(renderer, &headHitbox);

        player.render(renderer, ticks);
        SDL_RenderPresent(renderer);

        // Collision Block
        SDL_Rect playerHitBox = player.feet_hitbox();
        ground.h /= 10; // this whole things have to change... the colliders don't actually restrict player's movements
        ground1.h /= 10;
        ground2.h /= 10;
        ground3.h /= 10;

        if (SDL_HasIntersection(&ground, &playerHitBox))
        {
            player.is_grounded = true;
        }
        else if (SDL_HasIntersection(&ground2, &playerHitBox))
        {
            player.is_grounded = true;
        }
        else if (SDL_HasIntersection(&ground3, &playerHitBox))
        {
            player.is_grounded = true;
        }
        else if (SDL_HasIntersection(&ground1, &playerHitBox))
        {
            player.is_grounded = true;
        }
        else
        {
            player.is_grounded = false;
        }
        sleep_ms(20);
    }

    player.destroy();
}
