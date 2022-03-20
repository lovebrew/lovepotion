#include "modules/window/window.h"

using namespace love;

#include "common/exception.h"
#include "deko3d/deko.h"

#include "common/screen.h"

Window::Window()
{
    this->fullscreenModes = { { Screen::HANDHELD_WIDTH, Screen::HANDHELD_HEIGHT },
                              { Screen::DOCKED_WIDTH, Screen::DOCKED_HEIGHT } };
}

void Window::GetWindow(int& width, int& height)
{
    std::pair<uint32_t, uint32_t> size;
    ::deko3d::Instance().OnOperationMode(size);

    width  = size.first;
    height = size.second;

    this->OnSizeChanged(width, height);
}

bool Window::CreateWindowAndContext()
{
    std::pair<uint32_t, uint32_t> size;
    ::deko3d::Instance().OnOperationMode(size);

    this->OnSizeChanged(size.first, size.second);

    return true;
}

Window::DisplaySize Window::GetDesktopSize()
{
    auto operationMode = appletGetOperationMode();

    return this->fullscreenModes[operationMode];
}

void Window::OnSizeChanged(int width, int height)
{
    if (this->graphics.Get())
        this->graphics->SetMode(width, height);

    const Rect newViewport = { 0, 0, width, height };

    ::deko3d::Instance().SetViewport(newViewport);
    ::deko3d::Instance().SetScissor(newViewport, false);
}

int Window::GetDisplayCount()
{
    return 1;
}
