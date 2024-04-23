#pragma once

#include "common/screen.hpp"

#include "driver/display/Framebuffer.tcc"

#include <3ds.h>
#include <citro3d.h>

namespace love
{
    class Framebuffer : public FramebufferBase
    {
      public:
        Framebuffer();

        void create(const ScreenInfo& info);

        void destroy();

        void setViewport(const Rect& viewport);

        void setScissor(const Rect& scissor);

        C3D_RenderTarget* get() const
        {
            return this->target;
        }

        C3D_Mtx& getModelView()
        {
            return this->modelView;
        }

        C3D_Mtx& getProjection()
        {
            return this->projection;
        }

      private:
        static constexpr auto FORMAT = GPU_RB_DEPTH16;

        static constexpr auto DISPLAY_TRANSFER_FLAGS =
            GX_TRANSFER_FLIP_VERT(0) | GX_TRANSFER_OUT_TILED(0) | GX_TRANSFER_RAW_COPY(0) |
            GX_TRANSFER_IN_FORMAT(GX_TRANSFER_FMT_RGBA8) |
            GX_TRANSFER_OUT_FORMAT(GX_TRANSFER_FMT_RGB8) |
            GX_TRANSFER_SCALING(GX_TRANSFER_SCALE_NO);

        C3D_RenderTarget* target;

        C3D_Mtx projection;
        C3D_Mtx modelView;
    };
} // namespace love
