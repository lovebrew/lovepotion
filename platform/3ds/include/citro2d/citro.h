#pragma once

#include "common/mmath.h"
#include "objects/canvas/canvas.h"

class citro2d
{
    public:
        citro2d();

        ~citro2d();

        void BindFramebuffer(love::Canvas * canvas = nullptr);

        void ClearColor(const Colorf & color);

        void SetScissor(GPU_SCISSORMODE mode, const love::Rect & scissor, int screenWidth, bool canvasActive);

        void SetStencil(GPU_TESTFUNC compare, int value);

        void Present();

    private:
        std::vector<C3D_RenderTarget *> targets;
        C3D_RenderTarget * current;

        bool inFrame = false;

        void EnsureInFrame();
};

extern ::citro2d c2d;