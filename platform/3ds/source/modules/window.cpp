#include "modules/window/window.h"

using namespace love;

static constexpr std::array<Window::DisplaySize, 2> displaySizes = { { { 400, 240 },
                                                                       { 320, 240 } } };

Window::Window() : open(false)
{}

Window::~Window()
{
    this->graphics.Set(nullptr);
    this->open = false;
}

void Window::SetGraphics(Graphics* g)
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

void Window::OnSizeChanged(int width, int height)
{}

const std::array<Window::DisplaySize, 2>& Window::GetFullscreenModes()
{
    return displaySizes;
}

bool Window::IsOpen()
{
    return this->open;
}
