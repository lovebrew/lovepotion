#include "common/runtime.h"
#include "citro2d/citro.h"

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