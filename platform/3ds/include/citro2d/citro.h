#pragma once

#include "common/mmath.h"

class citro2d
{
    public:
        void SetScissor(GPU_SCISSORMODE mode, const love::Rect & scissor, int screenWidth, bool canvasActive);

        void SetStencil(GPU_TESTFUNC compare, int value);
};

extern ::citro2d c2d;