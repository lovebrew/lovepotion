#include "common/Exception.hpp"

#include "driver/display/Framebuffer.hpp"

namespace love
{
    Framebuffer::Framebuffer() : target(nullptr)
    {
        Mtx_Identity(&modelView);
    }

    void Framebuffer::create(const ScreenInfo& info)
    {
        const auto side = (gfxIs3D() && info.name == "right") ? GFX_RIGHT : GFX_LEFT;

        this->target = C3D_RenderTargetCreate(info.width, info.height, COLOR_FORMAT, DEPTH_FORMAT);

        if (!this->target)
            throw love::Exception("Failed to create render target '{:s}'.", info.name);

        const auto screen = (gfxScreen_t)info.id;
        C3D_RenderTargetSetOutput(this->target, screen, side, DISPLAY_TRANSFER_FLAGS);

        this->width  = info.width;
        this->height = info.height;

        // clang-format off
        Mtx_OrthoTilt(&this->projection, 0.0f, (float)this->width, (float)this->height, 0.0f, Z_NEAR, Z_FAR, true);
        // clang-format on

        this->viewport = { 0, 0, this->width, this->height };
        this->scissor  = { 0, 0, this->width, this->height };
    }

    void Framebuffer::destroy()
    {
        if (this->target != nullptr)
        {
            C3D_RenderTargetDelete(this->target);
            this->target = nullptr;
        }
    }

    void Framebuffer::calculateBounds(const Rect& bounds, Rect& out, const int width, const int height)
    {
        // clang-format off
        const uint32_t left   = height > (bounds.y + bounds.h) ? height - (bounds.y + bounds.h) : 0;
        const uint32_t top    = width  > (bounds.x + bounds.w) ? width - (bounds.x + bounds.w) : 0;
        const uint32_t right  = height - bounds.y;
        const uint32_t bottom = width  - bounds.x;
        // clang-format on

        out = { (int)left, (int)top, (int)right, (int)bottom };
    }

    void Framebuffer::setScissor(const Rect& scissor)
    {
        const bool enabled   = scissor != Rect::EMPTY;
        GPU_SCISSORMODE mode = enabled ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

        Rect result {};
        calculateBounds(scissor, result, this->width, this->height);

        this->scissor = result;
        C3D_SetScissor(mode, result.y, result.x, result.h, result.w);
    }
} // namespace love
