#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_scancode.h>
#include <SDL2/SDL_ttf.h>
#include "src/Engine.h"
#include "src/platformer.h"
#include "src/levelOne.h"
#include "src/utils.h"

#ifdef WIN32
#include <windows.h>
#endif

#define DEBUG true

int main(int argc, char *argv[])
{
    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
    SDL_Window *window = SDL_CreateWindow("Brahlers", 30, 30, WINDOW_WIDTH, WINDOW_HIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);

    //auto testLevel = GetTestStateLevel(renderer);
    //auto levelOneState = GetLevelOneState(renderer);
    auto levelOne_Scene = new Scene1(renderer);
    levelOne_Scene->start(renderer);
    auto editor = new Editor(levelOne_Scene);

    bool running = true;
    bool editMode = false;
    int levelNumber = 1;
    while (running)
    {
        SDL_Event ev;
        while (SDL_PollEvent(&ev))
        {
            if ((SDL_QUIT == ev.type) ||
                (SDL_KEYDOWN == ev.type && SDL_SCANCODE_ESCAPE == ev.key.keysym.scancode))
            {
                running = false;
                break;
            }

            if (SDL_KEYDOWN == ev.type) {
                if(SDL_SCANCODE_0 == ev.key.keysym.scancode)
                    levelNumber = 0;
                else if (SDL_SCANCODE_1 == ev.key.keysym.scancode)
                    levelNumber = 1;
                
                // EDIT MODE
                if(SDL_SCANCODE_F1 == ev.key.keysym.scancode) {
                    editMode = !editMode;
                }
            }
        }

        // THIS IS ONLY FOR DEBUGING
        // we should load a scene and destroy other scens so we dont load the whole game in ram
        if (!editMode) {
            switch (levelNumber) {
                //case 0: UpdateTestLevel(testLevel); break;
                case 1: levelOne_Scene->update(); 
                //default: UpdateTestLevel(testLevel); break;
            }
        }else {
            editor->update();
        }
        SDL_RenderPresent(renderer);
    }
    //DestroyTestLevel(testLevel);
    levelOne_Scene->destroy();

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0;
}
