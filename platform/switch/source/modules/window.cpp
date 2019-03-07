#include "common/runtime.h"

#include "modules/window_common.h"
#include "modules/window.h"

SDL_Window * WINDOW;
SDL_Renderer * RENDERER;

void Window::Initialize()
{
    Uint32 windowFlags = SDL_WINDOW_FULLSCREEN;
    WINDOW = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, windowFlags);

    if (!WINDOW)
        SDL_Quit();

    Uint32 rendererFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
    RENDERER = SDL_CreateRenderer(WINDOW, 0, rendererFlags);

    if (!RENDERER)
        SDL_Quit();

    SDL_SetRenderDrawBlendMode(RENDERER, SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
}

SDL_Renderer * Window::GetRenderer()
{
    return RENDERER;
}

void Window::Exit()
{
    SDL_DestroyRenderer(RENDERER);
    SDL_DestroyWindow(WINDOW);
}