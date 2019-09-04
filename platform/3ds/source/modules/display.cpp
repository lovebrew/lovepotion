#include "common/runtime.h"
#include "modules/display.h"

#include "modules/graphics.h"

void Display::Initialize()
{
    renderTargets = {
        C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT),
        C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT),
        C2D_CreateScreenTarget(GFX_TOP, GFX_RIGHT)
    };
}

bool Display::IsRenderingScreen(gfxScreen_t screen)
{
    return screen == rendering;
}

bool Display::IsRenderingSide(gfx3dSide_t side)
{
    return side == renderSide;
}

void Display::Clear(uint i)
{
    Color backgColor = Graphics::GetBackgroundColor();

    if (i == 0)
        renderSide = GFX_LEFT;
    else if (i == 1)
        renderSide = GFX_RIGHT;

    C2D_TargetClear(GetRenderer(i), Graphics::ConvertColor(backgColor));
    C2D_SceneBegin(GetRenderer(i));
}

C3D_RenderTarget * Display::GetRenderer(uint i)
{
    rendering = (gfxScreen_t)i;
    return renderTargets[i];
}
