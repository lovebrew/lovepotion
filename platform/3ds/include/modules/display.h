#pragma once

/*
** display.h
** @brief   : handles the actual renderer and window stuff
** @note    : this is for 3DS and it's gonna be a wild ride™
*/

class Display
{
    public:
        Display() = delete;

        static void Initialize();

        static bool IsRenderingScreen(gfxScreen_t screen);
        
        static void Clear(uint i);

        static C3D_RenderTarget * GetRenderer(uint i);

        static void Exit() {};

    private:
        static inline std::array<C3D_RenderTarget *, 3> renderTargets = { nullptr };

        static inline gfxScreen_t rendering = GFX_TOP;
};
