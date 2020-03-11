#include "common/runtime.h"
#include "modules/window/window.h"

using namespace love;

#include <SDL.h>
#include "common/exception.h"

Window::Window() : window(nullptr),
                   open(false)

{
    if (SDL_Init(SDL_INIT_VIDEO) != 0)
        throw love::Exception("Could not initialize SDL video subsystem (%s)", SDL_GetError());

    this->displaySizes =
    {
        {1280, 720}
    };
}

Window::~Window()
{
    if (this->targets[0])
        SDL_DestroyRenderer(this->targets[0]);

    if (this->window)
        SDL_DestroyWindow(this->window);

    SDL_QuitSubSystem(SDL_INIT_VIDEO);

    this->open = false;
}

Renderer * Window::GetRenderer()
{
    return this->targets[0];
}

void Window::SetScreen(size_t screen)
{}

bool Window::SetMode()
{
    this->window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_FULLSCREEN);

    if (!this->window)
        return false;

    u32 HW_ACCEL = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;

    this->targets = {
        SDL_CreateRenderer(this->window, 0, HW_ACCEL)
    };

    if (!this->targets[0])
        return false;

    SDL_SetRenderDrawBlendMode(this->targets[0], SDL_BLENDMODE_BLEND);

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");

    this->open = true;

    return true;
}

void Window::Clear(Color * color)
{
    // clear the background to the specified color
    SDL_SetRenderDrawColor(this->GetRenderer(), color->r, color->g, color->b, color->a);
    SDL_RenderClear(this->GetRenderer());

    // set the blending color for textures n stuff
    auto foreground = this->graphics.Get()->GetColor();
    SDL_SetRenderDrawColor(this->GetRenderer(), foreground.r, foreground.g, foreground.b, foreground.a);
}

void Window::Present()
{
    SDL_RenderPresent(this->GetRenderer());
}

std::vector<std::pair<int, int>> & Window::GetFullscreenModes()
{
    return this->displaySizes;
}

int Window::GetDisplayCount()
{
    return 1;
}

bool Window::IsOpen()
{
    return this->open;
}
