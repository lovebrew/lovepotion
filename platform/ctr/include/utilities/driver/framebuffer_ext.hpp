#pragma once

#include <objects/shader_ext.hpp>

#include <utilities/driver/renderer/framebuffer.tcc>

#include <citro3d.h>

namespace love
{
    template<>
    class Framebuffer<Console::CTR> : public Framebuffer<Console::ALL>
    {
      public:
        Framebuffer();

        void Create(Screen screen);

        void Destroy();

        void UseProjection(Shader<Console::CTR>::Uniforms uniforms);

        void SetViewport(const Rect& viewport = Rect::EMPTY, bool canvasActive = false);

        void SetScissor(const Rect& scissor = Rect::EMPTY, bool canvasActive = false);

        C3D_RenderTarget* GetTarget()
        {
            return this->target;
        }

        C3D_Mtx& GetModelView()
        {
            return this->modelView;
        }

        C3D_Mtx& GetProjView()
        {
            return this->projView;
        }

      private:
        static constexpr auto DISPLAY_FLAGS = GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) |
                                              GX_TRANSFER_RAW_COPY(0) |
                                              GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
                                              GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
                                              GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);

        const Rect CalculateBounds(const Rect& rect);

        void SetSize(int width, int height, gfxScreen_t screen, gfx3dSide_t side);

        C3D_RenderTarget* target;

        C3D_Mtx modelView;
        C3D_Mtx projView;
    };
} // namespace love
