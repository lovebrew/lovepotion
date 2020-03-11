#include "common/runtime.h"
#include "modules/window/window.h"

using namespace love;

Window::Window() : open(false)
{
    gfxInitDefault();
    gfxSet3D(true);

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        svcBreak(USERBREAK_PANIC);

    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
        svcBreak(USERBREAK_PANIC);

    C2D_Prepare();

    this->displaySizes =
    {
        { 400, 240 },
        { 400, 240 },
        { 320, 240 }
    };
}

Window::~Window()
{
    this->graphics.Set(nullptr);

    C2D_Fini();
    C3D_Fini();
    gfxExit();

    this->open = false;
}

bool Window::SetMode()
{
    this->targets =
    {
        C2D_CreateScreenTarget(GFX_TOP,    GFX_LEFT),
        C2D_CreateScreenTarget(GFX_TOP,    GFX_RIGHT),
        C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT)
    };

    this->open = true;

    return true;
}

void Window::Clear(Color * color)
{
    C3D_FrameBegin(C3D_FRAME_SYNCDRAW);

    for (size_t index = 0; index < targets.size(); index++)
        C2D_TargetClear(targets[index], C2D_Color32f(color->r, color->g, color->b, color->a));
}

void Window::Present()
{
    C3D_FrameEnd(0);
}

Renderer * Window::GetRenderer()
{
    return nullptr;
}

int Window::GetDisplayCount()
{
    return 3;
}

std::vector<std::pair<int, int>> & Window::GetFullscreenModes()
{
    return this->displaySizes;
}

bool Window::IsOpen()
{
    return this->open;
}

void Window::SetScreen(size_t screen)
{
    this->currentDisplay = std::clamp((int)screen - 1, 0, 2);

    C2D_SceneBegin(this->targets[this->currentDisplay]);
}
