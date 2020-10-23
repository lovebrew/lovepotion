#include "common/runtime.h"
#include "modules/window/window.h"

using namespace love;

#include "deko3d/deko.h"
#include "common/exception.h"

Window::Window() : window(nullptr),
                   open(false)

{
    this->displaySizes =
    {
        {1280, 720}
    };

    this->targets.reserve(1);
}

Window::~Window()
{
    this->graphics.Set(nullptr);

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
    return this->open = true;
}

void Window::Clear(Color *)
{}

void Window::SetRenderer(Canvas * canvas)
{}

void Window::SetGraphics(Graphics * g)
{
    this->graphics.Set(g);
}

void Window::Present()
{}

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
