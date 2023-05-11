#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <cstdlib>
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <vector>

#ifdef WIN32
#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
#include <time.h>   // for nanosleep
#else
#include <unistd.h> // for usleep
#endif

#define SDLCOLOR_GREEN SDL_Color{86,125,70,255}
#define WINDOW_WIDTH 800
#define WINDOW_HIGHT 600

void sleep_ms(int milliseconds){ // cross-platform sleep function
#ifdef WIN32
    Sleep(milliseconds);
#elif _POSIX_C_SOURCE >= 199309L
    struct timespec ts;
    ts.tv_sec = milliseconds / 1000;
    ts.tv_nsec = (milliseconds % 1000) * 1000000;
    nanosleep(&ts, NULL);
#else
    if (milliseconds >= 1000)
      sleep(milliseconds / 1000);
    usleep((milliseconds % 1000) * 1000);
#endif
}

class Player{
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

        Player(SDL_Renderer *renderer,char *sprite) {
            image = IMG_Load(sprite);
            texture = SDL_CreateTextureFromSurface(renderer, image);
        }

        SDL_Rect hitbox() {
            return SDL_Rect {x,y+1,52,64};
        }

        void move(int x,int y){
            if(x < 0) {
                playerDir = -1;
            }else if(x > 0) {
                playerDir = +1;
            }
            this->x += x * speed * 1.5;
            this->y += y * speed * 1.5;
        }

        void destroy() {
            SDL_DestroyTexture(texture);
            SDL_FreeSurface(image);
        }

        void render(SDL_Renderer *renderer,Uint32 ticks) {
            const Uint8 *keystates = SDL_GetKeyboardState(NULL);
            if(keystates[SDL_SCANCODE_LEFT]) {
                is_player_moving = true;
                move(-1,0);
            }else if(keystates[SDL_SCANCODE_RIGHT]) {
                is_player_moving = true;
                move(1,0);
            }else {
                is_player_moving = false;
            }

            if(keystates[SDL_SCANCODE_UP] && !is_jumping && is_grounded){
                is_jumping = true;
            }

            if(is_jumping) {
                if(playerDistFromGround <= jumpHight){
                    move(0,-1);
                    playerDistFromGround++;
                    int sp_pos = 35;
                    SDL_Rect srcrect = {sp_pos,48,14,16};
                    SDL_Rect dstrect = {x,y,56,64};
                    SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0? SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL));
                    return;
                }else{
                    is_jumping = false;
                }
            }else {
                if(is_grounded){
                    playerDistFromGround = 0;
                    is_jumping = false;
                }else {
                    move(0,1);
                    int sp_pos = true ? 17 : 35;
                    SDL_Rect srcrect = {sp_pos,48,14,16};
                    SDL_Rect dstrect = {x,y,56,64};
                    SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0? SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL));
                    return;
                }
            }
            
            if(is_player_moving){
                Uint32 sprite = (ticks / 100) % 6;
                int sp_pos = (sprite * 16) + 19;
                SDL_Rect srcrect = {sp_pos,32,13,16};
                SDL_Rect dstrect = {x,y,52,64};
                SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0? SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL));
            }else {
                Uint32 sprite = (ticks / 300) % 4;
                int sp_pos = (sprite * 16) + 19;
                SDL_Rect srcrect = {sp_pos,16,13,16};
                SDL_Rect dstrect = {x,y,52,64};
                SDL_RenderCopyEx(renderer, texture, &srcrect, &dstrect, 0, NULL, (playerDir > 0? SDL_FLIP_NONE:SDL_FLIP_HORIZONTAL));
            }
        }
};


int main() {
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    SDL_Window *window = SDL_CreateWindow("ItGetsHarder", 100, 100, WINDOW_WIDTH, WINDOW_HIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

    bool running = true;

    char sprite[] = "./Old-hero.png";
    Player player(renderer,sprite);
    while(running) {
        Uint32 ticks = SDL_GetTicks();
        SDL_Event ev;
        while(SDL_PollEvent(&ev)){
            if((SDL_QUIT == ev.type) ||
                    (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode)) {
                running = false;
                break;
            }
        }

        // Render Block
        SDL_SetRenderDrawColor(renderer, 168, 230, 255, 255);
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer,86,125,70,255);
        SDL_Rect ground {0,300,200,50};
        SDL_RenderFillRect(renderer, &ground);
        SDL_SetRenderDrawColor(renderer,86,125,70,255);
        SDL_Rect ground2 {260,360,180,50};
        SDL_RenderFillRect(renderer, &ground2);
        SDL_SetRenderDrawColor(renderer,86,125,70,255);
        SDL_Rect ground3 {500,420,400,50};
        SDL_RenderFillRect(renderer, &ground3);
        player.render(renderer,ticks);
        SDL_RenderPresent(renderer);

        // Collision Block
        //SDL_Rect playerHitBox = {player.x,player.y,56,64};
        SDL_Rect playerHitBox = player.hitbox();
        if(SDL_HasIntersection(&ground, &playerHitBox)){
            player.is_grounded = true;
        } 
        else if(SDL_HasIntersection(&ground2, &playerHitBox)){
            player.is_grounded = true;
        }
        else if(SDL_HasIntersection(&ground3, &playerHitBox)){
            player.is_grounded = true;
        }
        else{
            player.is_grounded = false;
        }
        sleep_ms(30);
    }
   
    player.destroy();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
