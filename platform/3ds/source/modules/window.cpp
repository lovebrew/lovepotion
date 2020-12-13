#include "common/runtime.h"
#include "modules/window/window.h"

using namespace love;

Window::Window() : open(false)
{
    this->displaySizes =
    {
        { 400, 240 },
        { 320, 240 }
    };
}

Window::~Window()
{
    this->graphics.Set(nullptr);
    this->open = false;
}

void Window::SetGraphics(Graphics * g)
{
    this->graphics.Set(g);
}

bool Window::SetMode()
{
    return this->open = true;
}

int Window::GetDisplayCount()
{
    return 2;
}

std::vector<std::pair<int, int>> & Window::GetFullscreenModes()
{
    return this->displaySizes;
}

bool Window::IsOpen()
{
    return this->open;
}
