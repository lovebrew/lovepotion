#include "modules/window/window.h"

using namespace love;

#include "common/exception.h"
#include "deko3d/deko.h"

static constexpr std::array<Window::DisplaySize, 2> displaySizes = { { { 1920, 1080 },
                                                                       { 1280, 720 } } };

Window::Window() : open(false)
{
    auto size = ::deko3d::Instance().OnOperationMode(appletGetOperationMode());
    this->OnSizeChanged(size.first, size.second);
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

void Window::SetGraphics(Graphics* g)
{
    this->graphics.Set(g);
}

void Window::OnSizeChanged(int width, int height)
{
    if (this->graphics.Get())
        this->graphics->Resize(width, height);

    const Rect newViewport = { 0, 0, width, height };

    ::deko3d::Instance().SetViewport(newViewport);
    ::deko3d::Instance().SetScissor(newViewport, false);
}

const std::array<Window::DisplaySize, 2>& Window::GetFullscreenModes()
{
    return displaySizes;
}

int Window::GetDisplayCount()
{
    return 1;
}

bool Window::IsOpen()
{
    return this->open;
}
