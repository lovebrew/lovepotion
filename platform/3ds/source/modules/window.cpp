#include "modules/window/window.h"

using namespace love;

Window::Window()
{
    this->fullscreenModes = { { 400, 240 }, { 320, 240 } };
}

int Window::GetDisplayCount()
{
    return 2;
}
