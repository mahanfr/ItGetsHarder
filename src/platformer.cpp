#include <SDL2/SDL.h>
// wtf is all this?

// #include <SDL2/SDL_events.h>
// #include <SDL2/SDL_keyboard.h>
// #include <SDL2/SDL_keycode.h>
// #include <SDL2/SDL_pixels.h>
// #include <SDL2/SDL_render.h>
// #include <SDL2/SDL_scancode.h>
// #include <SDL2/SDL_surface.h>
// #include <SDL2/SDL_timer.h>
// #include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>
#include "utils.h"
#include "platformer.h"
#define MUS_PATH ".././assets/jump.wav"
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
    int x = 100;
    int y = 200;
    bool is_player_moving = false;
    bool is_grounded = false;
    int playerDir = +1;
    // The music that will be played
    Mix_Music *music = NULL;

    // The sound effects that will be used
    Mix_Chunk *scratch = NULL;
    Mix_Chunk *high = NULL;
    Mix_Chunk *med = NULL;
    Mix_Chunk *low = NULL;
    Player(SDL_Renderer *renderer, char *sprite)
    {
        image = IMG_Load(sprite);
        texture = SDL_CreateTextureFromSurface(renderer, image);
    }

    SDL_Rect hitbox()
    {
        return SDL_Rect{x, y + 1, 52, 64};
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
    }

    void playJumpSound()
    {
        SDL_AudioSpec wav_spec;
        Uint32 wav_length;
        Uint8 *wav_buffer;
        SDL_AudioSpec wavSpec;
        /* Load the WAV */
        try
        {

            if (SDL_Init(SDL_INIT_AUDIO) < 0)
            {
                printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
            }
            if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
            {
                printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
            }

            Mix_Chunk *sound = Mix_LoadWAV(MUS_PATH);
            if (sound == NULL)
            {
                printf("Failed to load sound file! SDL_mixer Error: %s\n", Mix_GetError());
            }

            Mix_PlayChannel(-1, sound, 0);

            while (Mix_Playing(-1) != 0)
            {
                SDL_Delay(100);
            }

            Mix_FreeChunk(sound);

            Mix_CloseAudio();

            // SDL_Quit();
            // if (SDL_LoadWAV(MUS_PATH, &wav_spec, &wav_buffer, &wav_length) == NULL)
            // {
            //     fprintf(stderr, "Could not open jump.wav: %s\n", SDL_GetError());
            // }

            // if (wavSpec.format != AUDIO_S16LSB)
            // {
            //     printf("Error: Unsupported audio format\n");
            //     SDL_FreeWAV(wav_buffer);
            // }
            // SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
            // if (deviceId == 0)
            // {
            //     printf("Error: %s\n", SDL_GetError());
            // }

            // SDL_QueueAudio(deviceId, wav_buffer, wav_length);
            // SDL_PauseAudioDevice(deviceId, 0);

            // SDL_Delay(5000);

            // SDL_CloseAudioDevice(deviceId);
            // SDL_FreeWAV(wav_buffer);
            // SDL_Quit();

            // /* Free It */
            // SDL_FreeWAV(wav_buffer);
        }
        catch (const std::exception &e)
        {
            printf("Error: %s\n", e);
        }
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
            playJumpSound();
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

    bool running = true;

    char sprite[] = ".././assets/Old-hero.png";
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
        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_Rect ground2{260, 360, 180, 50};
        SDL_RenderFillRect(renderer, &ground2);
        SDL_SetRenderDrawColor(renderer, 86, 125, 70, 255);
        SDL_Rect ground3{500, 420, 400, 50};
        SDL_RenderFillRect(renderer, &ground3);
        player.render(renderer, ticks);
        SDL_RenderPresent(renderer);

        // Collision Block
        // SDL_Rect playerHitBox = {player.x,player.y,56,64};
        SDL_Rect playerHitBox = player.hitbox();
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
        else
        {
            player.is_grounded = false;
        }
        sleep_ms(30);
    }

    player.destroy();
}
