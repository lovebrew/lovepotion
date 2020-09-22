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

    this->targets.reserve(1);
}

Window::~Window()
{
    if (this->open)
    {
        if (this->targets.back())
        {
            SDL_DestroyRenderer(this->targets.back());

            this->targets.back() = nullptr;
        }

        if (this->window)
        {
            SDL_DestroyWindow(this->window);

            this->window = nullptr;
        }

        this->graphics.Set(nullptr);

        SDL_QuitSubSystem(SDL_INIT_VIDEO);
    }

    this->open = false;
}

Renderer * Window::GetRenderer()
{
    return this->targets.back();
}

void Window::SetScreen(size_t screen)
{
    // this never goes above 1 but.. not risking it
    this->currentDisplay = std::max(screen - 1, (size_t)0);
}

bool Window::SetMode()
{
    this->window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, SDL_WINDOW_FULLSCREEN);

    if (!this->window)
        return false;

    this->targets.push_back(SDL_CreateRenderer(this->window, 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC));

    if (!this->targets.back())
        return false;

    SDL_SetRenderDrawBlendMode(this->targets.back(), SDL_BLENDMODE_BLEND);

    this->open = true;

    return true;
}

void Window::Clear(Color * color)
{
    // clear the background to the specified color
    SDL_SetRenderDrawColor(this->targets.back(), color->r, color->g, color->b, SDL_ALPHA_OPAQUE);
    this->EnsureInFrame();

    auto foreground = this->graphics->AdjustColor(this->graphics.Get()->GetColor());
    SDL_SetRenderDrawColor(this->GetRenderer(), foreground.r, foreground.g, foreground.b, foreground.a);
}

void Window::EnsureInFrame()
{
    if (!this->inFrame)
    {
        SDL_RenderClear(this->targets.back());
        this->inFrame = true;
    }
}

void Window::SetRenderer(Canvas * canvas)
{
    this->canvas.Set(canvas);

    if (this->canvas)
    {
        SDL_SetRenderTarget(this->targets.back(), canvas->GetTextureHandle());

        if (!canvas->HasFirstClear())
            this->canvas->Clear({0, 0, 0, 0});
    }
    else
        SDL_SetRenderTarget(this->targets.back(), NULL);
}

void Window::SetGraphics(Graphics * g)
{
    this->graphics.Set(g);
}

void Window::Present()
{
    SDL_RenderPresent(this->targets.back());
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
