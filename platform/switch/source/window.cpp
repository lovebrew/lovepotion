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
}

Window::~Window()
{
    this->graphics.Set(nullptr);

    this->open = false;
}


bool Window::SetMode()
{
    return this->open = true;
}

void Window::SetGraphics(Graphics * g)
{
    this->graphics.Set(g);
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
