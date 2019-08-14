#include "common/runtime.h"
#include "modules/display.h"

void Display::Initialize()
{
    u32 FULLSCREEN = SDL_WINDOW_FULLSCREEN;
    u32 CENTERED = SDL_WINDOWPOS_CENTERED;

    // title x_pos y_pos width height window_flags
    WINDOW = SDL_CreateWindow(NULL, CENTERED, CENTERED, 1280, 720, FULLSCREEN);

    if (!WINDOW)
        SDL_Quit();

    u32 HW_ACCEL = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    RENDERER = SDL_CreateRenderer(WINDOW, 0, HW_ACCEL);

    if (!RENDERER)
        SDL_Quit();

    SDL_SetRenderDrawBlendMode(RENDERER, SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}

SDL_Renderer * Display::GetRenderer()
{
    return RENDERER;
}

void Display::Exit()
{
    if (RENDERER)
        SDL_DestroyRenderer(RENDERER);
    
    if (WINDOW)
        SDL_DestroyWindow(WINDOW);
}
