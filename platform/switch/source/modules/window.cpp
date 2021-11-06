#include "modules/window/window.h"

using namespace love;

#include "common/exception.h"
#include "deko3d/deko.h"

Window::Window()
{
    this->fullscreenModes = { { 1280, 720 }, { 1920, 1080 } };
}

void Window::GetWindow(int& width, int& height)
{
    auto size = ::deko3d::Instance().OnOperationMode(appletGetOperationMode());

    width  = size.first;
    height = size.second;

    this->OnSizeChanged(width, height);
}

bool Window::CreateWindowAndContext()
{
    auto size = ::deko3d::Instance().OnOperationMode(appletGetOperationMode());
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
