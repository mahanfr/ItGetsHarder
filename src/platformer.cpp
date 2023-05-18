#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_mixer.h>
#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <sstream>
#include <stdio.h>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "platformer.h"
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
#define DEFAULT_COLOR    \
    {                    \
        86, 125, 70, 255 \
    }

typedef struct Color
{
    int r, g, b, a;
} Color;

// all Obsticles in one place
SDL_Rect obsticle_array[5] = {{0, 300, 200, 50}, {260, 380, 180, 50}, {500, 420, 400, 50}, {200, 500, 400, 50}, {200, 230, 400, 50}};

class Obsticles
{
public:
    Obsticles(SDL_Renderer *renderer, Color color = DEFAULT_COLOR)
    {
        SDL_SetRenderDrawColor(renderer, 168, 230, 255, 255);
        SDL_RenderClear(renderer);
        for (SDL_Rect rectangle : obsticle_array)
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rectangle);
        }
    }
    void rectangleDrawer(SDL_Renderer *renderer, SDL_Rect shape, Color color = DEFAULT_COLOR)
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &shape);
    }
};

bool playerAndRectangleCollisioinDetector(SDL_Rect hitbox, SDL_Rect rectangle)
{
    return (SDL_HasIntersection(&hitbox, &rectangle));
}

class Player
{
private:
    SDL_Surface *image;
    SDL_Texture *texture;
    int jumpHight = 15;
    int playerDistFromGround = 0;
    bool is_jumping = false;
    bool is_punching = false;
    bool punching_cooldown = 0;
    Uint32 punching_duration = 0;
    int punch_afp = 0;
    int x = 100;
    int y = 200;
    int speed = 4;

public:
    bool is_player_moving = false;
    // Changed it to dynamicly check for ground collition
    // might need to revert for optimization
    // bool is_grounded = false;
    int playerDir = +1;
    Mix_Chunk *jumpingSound;
    float health = 100;

    Player();

    Player(SDL_Renderer *renderer, char *sprite)
    {
        image = IMG_Load(sprite);
        texture = SDL_CreateTextureFromSurface(renderer, image);
        jumpingSound = Mix_LoadWAV(MUS_PATH);
    }
    
    string get_player_health_display()
    {
        stringstream text;
        text.str("");
        text << "Health: " << health;
        auto res = text.str();
        return res;
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
    SDL_Rect left_hitbox()
    {
        return SDL_Rect{x, y + 5, 12, 50};
    }
    SDL_Rect right_hitbox()
    {
        return SDL_Rect{x + 30, y + 5, 12, 50};
    }

    void move(int x, int y)
    {
        if (x < 0)
            playerDir = -1;
        else if (x > 0)
            playerDir = +1;

        this->x += x * speed * 1.5;
        this->y += y * speed * 1.5;
    }

    void move_if_no_collide(int x, int y)
    {

        for (SDL_Rect rectangle : obsticle_array)
        {
            // trying to restrict player
            if (playerAndRectangleCollisioinDetector(left_hitbox(), rectangle) && x < 0)
                return;
            if (playerAndRectangleCollisioinDetector(head_hitbox(), rectangle) && y < 0)
            {
                move(x, -y);
                is_jumping = false;
                return;
            }
            if (playerAndRectangleCollisioinDetector(right_hitbox(), rectangle) && x > 0)
                return;
            if (playerAndRectangleCollisioinDetector(feet_hitbox(), rectangle) && y > 0)
                return;
            if (this->y > 500)
            {
                this->x = 100;
                this->y = 200;

                return;
            }
        }
        move(x, y);
    }

    bool is_grounded_dyn()
    {
        auto result = false;
        for (SDL_Rect rectangle : obsticle_array)
        {
            if (playerAndRectangleCollisioinDetector(feet_hitbox(), rectangle))
                return true;
        }
        return result;
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

    void render(SDL_Renderer *renderer)
    {
        Uint32 ticks = SDL_GetTicks();
        const Uint8 *keystates = SDL_GetKeyboardState(NULL);
        if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A])
        {
            is_player_moving = true;
            move_if_no_collide(-1, 0);
        }
        else if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D])
        {
            is_player_moving = true;
            move_if_no_collide(1, 0);
        }
        else
            is_player_moving = false;

        if ((keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_SPACE]) && !is_jumping && is_grounded_dyn())
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
            SDL_Rect dstrect = {x, y, 56, 64};
            SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
            return;
        }

        if (is_jumping)
        {
            if (playerDistFromGround == 1)
                playJumpSound();

            if (playerDistFromGround <= jumpHight)
            {
                move_if_no_collide(0, -1);
                playerDistFromGround++;
                int sp_pos = 17;
                SDL_Rect srcrect = {sp_pos, 48, 14, 16};
                SDL_Rect dstrect = {x, y, 56, 64};
                SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0 ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL));
                return;
            }
            else
                is_jumping = false;
        }
        else
        {
            if (is_grounded_dyn())
            {
                playerDistFromGround = 0;
                is_jumping = false;
            }
            else
            {
                move_if_no_collide(0, 1);
                int sp_pos = 35;
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

TestLevelState GetTestStateLevel(SDL_Renderer* renderer) {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
    {
        printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
    }
    char sprite[] = SPRITE_PATH;
    Player* player = new Player(renderer ,sprite);
    TestLevelState state{renderer, player};
    return state;
}

void UpdateTestLevel(TestLevelState state) {
   Obsticles Obsticles(state.renderer);
   state.player->render(state.renderer);
   SDL_RenderPresent(state.renderer);
}

void DestroyTestLevel(TestLevelState state) {
    state.player->destroy();
}
