#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <complex>
#include "levelOne.h"
#include "utils.h"

#ifdef _WIN32
#define BACKGROUND_IMAGE_PATH ".././assets/level-one/background.png"
#define FORGROUND_IMAGE_PATH ".././assets/level-one/forground.png"
#define COLUD_ONE_IMAGE_PATH ".././assets/level-one/cloud-one.png"
#define CLOUD_TWO_IMAGE_PATH ".././assets/level-one/cloud-two.png"
#else
#define BACKGROUND_IMAGE_PATH "./assets/level-one/background.png"
#define FORGROUND_IMAGE_PATH "./assets/level-one/forground.png"
#define CLOUD_ONE_IMAGE_PATH "./assets/level-one/cloud-one.png"
#define CLOUD_TWO_IMAGE_PATH "./assets/level-one/cloud-two.png"
#endif

class BackgroudLeyer{
private:
    SDL_Surface* image;
    SDL_Texture* texture;
public:
    BackgroudLeyer(SDL_Renderer *renderer, char *path) {
        image = IMG_Load(path);
        texture = SDL_CreateTextureFromSurface(renderer, image);
    }
    void render(SDL_Renderer *renderer) {
        SDL_Rect srcrect = {0, 0, image->w, image->h};
        SDL_Rect dstrect = {0, 0, WINDOW_WIDTH, WINDOW_HIGHT};
        SDL_RenderCopy(renderer, texture, &srcrect, &dstrect);
    }
    void destroy() {
        SDL_DestroyTexture(texture);
        SDL_FreeSurface(image);
    }
};

class Clouds {
private:
    int cloud1_x, cloud1_y;
    int cloud2_x, cloud2_y;
    SDL_Surface* cloud1 = IMG_Load(CLOUD_ONE_IMAGE_PATH);
    SDL_Surface* cloud2 = IMG_Load(CLOUD_TWO_IMAGE_PATH);
    SDL_Texture* cloud1_texture;
    SDL_Texture* cloud2_texture;
public:
    Clouds(SDL_Renderer* renderer){
        cloud1_texture = SDL_CreateTextureFromSurface(renderer, cloud1);
        cloud2_texture = SDL_CreateTextureFromSurface(renderer, cloud2);
        cloud1_x = rand() % WINDOW_WIDTH;
        cloud2_x = rand() % WINDOW_WIDTH;
        cloud1_y = rand() % 200;
        cloud2_y = rand() % 200;
    }

    void render(SDL_Renderer* renderer) {
        {
            SDL_Rect srcrect = {0, 0, cloud1->w, cloud1->h};
            SDL_Rect dstrect = {cloud1_x, cloud1_y, cloud1->w, cloud1->h};
            SDL_RenderCopy(renderer, cloud1_texture, &srcrect, &dstrect);
        }
        {
            SDL_Rect srcrect = {0, 0, cloud2->w, cloud2->h};
            SDL_Rect dstrect = {cloud2_x, cloud2_y, cloud2->w, cloud2->h};
            SDL_RenderCopy(renderer, cloud2_texture, &srcrect, &dstrect);
        }
        cloud1_x-=2;
        cloud2_x-=2;
        if(cloud1_x < -300) {
            cloud1_y = rand() % 200;
            cloud1_x = WINDOW_WIDTH + rand() % WINDOW_WIDTH / 2;
        }
        if(cloud2_x < -300) {
            cloud2_y = rand() % 200;
            cloud2_x = WINDOW_WIDTH + rand() % WINDOW_WIDTH / 2;
        }
    }

    void destroy() {
        SDL_DestroyTexture(cloud1_texture);
        SDL_DestroyTexture(cloud2_texture);
        SDL_FreeSurface(cloud1);
        SDL_FreeSurface(cloud2);
    }
};

class Background {
    SDL_Renderer *renderer;
    Clouds *clouds;
    BackgroudLeyer* background;
    BackgroudLeyer* forground;
public:
    Background(SDL_Renderer *renderer) {
        this-> renderer = renderer;
        clouds = new Clouds(renderer);
        char bPath[] = BACKGROUND_IMAGE_PATH;
        char fPath[] = FORGROUND_IMAGE_PATH;
        background = new BackgroudLeyer(renderer,bPath);
        forground  = new BackgroudLeyer(renderer,fPath);
    }
    void render() {
        background->render(renderer);
        clouds->render(renderer);
        forground->render(renderer);
    }
    void destroy() {
        forground->destroy();
        clouds->destroy();
        background->destroy();
    }
};


LevelOneState GetLevelOneState(SDL_Renderer* renderer) {
    Background* background = new Background(renderer);
    return LevelOneState{renderer,background};
}

void UpdateLevelOne(LevelOneState state){
    SDL_SetRenderDrawColor(state.renderer, 8, 166, 213, 255);
    SDL_RenderClear(state.renderer);
    state.background->render();
} 

void DestroyLevelOne(LevelOneState state){
    state.background->destroy();
}


