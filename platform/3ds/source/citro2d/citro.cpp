#include "common/runtime.h"
#include "citro2d/citro.h"

#include "modules/window/window.h"

citro2d::citro2d()
{
    gfxInitDefault();
    gfxSet3D(true);

    if (!C3D_Init(C3D_DEFAULT_CMDBUF_SIZE))
        svcBreak(USERBREAK_PANIC);

    if (!C2D_Init(C2D_DEFAULT_MAX_OBJECTS))
        svcBreak(USERBREAK_PANIC);

    C2D_Prepare();

    C3D_AlphaTest(true, GPU_GREATER, 0);

    this->targets =
    {
        C2D_CreateScreenTarget(GFX_TOP,    GFX_LEFT),
        C2D_CreateScreenTarget(GFX_TOP,    GFX_RIGHT),
        C2D_CreateScreenTarget(GFX_BOTTOM, GFX_LEFT)
    };
}

citro2d::~citro2d()
{
    C2D_Fini();
    C3D_Fini();
    gfxExit();
}

void citro2d::EnsureInFrame()
{
    if (!this->inFrame)
    {
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        this->inFrame = true;
    }
}

void citro2d::BindFramebuffer(love::Canvas * canvas)
{
    this->EnsureInFrame();

    if (canvas != nullptr)
        C2D_SceneBegin(canvas->GetRenderer());
    else
        C2D_SceneBegin(this->targets[love::Window::CURRENT_DISPLAY]);
}

void citro2d::ClearColor(const Colorf & color)
{
    C2D_TargetClear(this->targets[love::Window::CURRENT_DISPLAY], C2D_Color32f(color.r, color.g, color.b, color.a));
}

void citro2d::Present()
{
    if (this->inFrame)
    {
        C3D_FrameEnd(0);
        this->inFrame = false;
    }
}

void citro2d::SetScissor(GPU_SCISSORMODE mode, const love::Rect & scissor, int screenWidth, bool canvasActive)
{
    C3D_SetScissor(mode, 240 - (scissor.y + scissor.h), screenWidth - (scissor.x + scissor.w), 240 - scissor.y, screenWidth - scissor.x);
}

void citro2d::SetStencil(GPU_TESTFUNC compare, int value)
{
    bool enabled = (compare == GPU_ALWAYS) ? false : true;

    C3D_StencilTest(enabled, compare, value, 0xFFFFFFFF, 0xFFFFFFFF);
    C3D_StencilOp(GPU_STENCIL_KEEP, GPU_STENCIL_KEEP, GPU_STENCIL_KEEP);
}

::citro2d c2d;