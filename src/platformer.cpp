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
using namespace std;
#ifdef _WIN32
#define MUS_PATH ".././assets/jump.wav"
#define SPRITE_PATH ".././assets/Old-hero.png";
#else
#define MUS_PATH "./assets/jump.wav"
#define SPRITE_PATH "./assets/Old-hero.png";
#endif
#define WINDOW_WIDTH 800
#define WINDOW_HIGHT 600
typedef struct Color
{
    int r, g, b, a;
} Color;

// all Obsticles in one place
SDL_Rect obsticle_array[4] = {{0, 300, 200, 50}, {260, 360, 180, 50}, {500, 420, 400, 50}, {200, 500, 400, 50}};

class Obsticles
{
public:
    Obsticles(SDL_Renderer *renderer, Color color = {r : 86, g : 125, b : 70, a : 255})
    {
        SDL_SetRenderDrawColor(renderer, 168, 230, 255, 255);
        SDL_RenderClear(renderer);
        for (SDL_Rect rectangle : obsticle_array)
        {
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderFillRect(renderer, &rectangle);
        }
    }
    void rectangleDrawer(SDL_Renderer *renderer, SDL_Rect shape, Color color = {r : 86, g : 125, b : 70, a : 255})
    {
        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        SDL_RenderFillRect(renderer, &shape);
    }
};
bool playerAndRectangleCollisioinDetector(SDL_Rect hitbox, SDL_Rect rectangle)
{
    if (SDL_HasIntersection(&hitbox, &rectangle))
    {
        cout << "false";
        return true;
    }
    return false;
}

class Player
{
private:
    SDL_Surface *image;
    SDL_Texture *texture;
    int jumpHight = 15;
    int playerDistFromGround = 0;
    bool is_jumping = false;
    int x = 100;
    int y = 200;
    int speed = 4;

public:
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
        {
            playerDir = -1;
        }
        else if (x > 0)
        {
            playerDir = +1;
        }
        for (SDL_Rect rectangle : obsticle_array)
        {
            // trying to restrict player
            if (!playerAndRectangleCollisioinDetector(left_hitbox(), rectangle)) //&& x > this->x

                this->x += x * speed * 1.5; // x int e bad * 1.5 mikonim?

            if (!playerAndRectangleCollisioinDetector(right_hitbox(), rectangle)) //&& x < this->x

                this->x += x * speed * 1.5;

            // if (!playerAndRectangleCollisioinDetector(head_hitbox(), rectangle)) // && y > this->y

            //     this->y += y * speed * 1.5;
            if (this->y < 500)
                if (!playerAndRectangleCollisioinDetector(feet_hitbox(), rectangle))

                {
                    this->y += y * speed * 1.5;

                    cout << this->y << " + " << y << "\n";
                }
        }
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
        if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A])
        {
            is_player_moving = true;
            move(-1, 0);
        }
        else if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D])
        {
            is_player_moving = true;
            move(1, 0);
        }
        else
        {
            is_player_moving = false;
        }

        if ((keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W] || keystates[SDL_SCANCODE_SPACE]) && !is_jumping && is_grounded)
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
    // TODO add try catch for all sound stuff
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
        Obsticles Obsticles(renderer);

        Obsticles.rectangleDrawer(renderer, player.left_hitbox());
        Obsticles.rectangleDrawer(renderer, player.right_hitbox(), {0, 0, 255, 255});
        Obsticles.rectangleDrawer(renderer, player.feet_hitbox(), {255, 0, 0, 255});
        Obsticles.rectangleDrawer(renderer, player.head_hitbox(), {255, 0, 255, 255});

        player.render(renderer, ticks);
        SDL_RenderPresent(renderer);

        sleep_ms(40);
    }

    player.destroy();
}
