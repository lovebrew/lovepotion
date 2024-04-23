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

        this->target = C3D_RenderTargetCreate(info.width, info.height, GPU_RB_RGBA8, FORMAT);

        if (!this->target)
            throw love::Exception("Failed to create render target.");

        const auto screen = (gfxScreen_t)info.id;
        C3D_RenderTargetSetOutput(this->target, screen, side, Framebuffer::DISPLAY_TRANSFER_FLAGS);

        this->width  = info.width;
        this->height = info.height;
    }

    void Framebuffer::destroy()
    {
        if (this->target != nullptr)
        {
            C3D_RenderTargetDelete(this->target);
            this->target = nullptr;
        }
    }

    static const Rect calculateBounds(const Rect& bounds, int width, int height)
    {
        // clang-format off
        const uint32_t left   = height > (bounds.y + bounds.h) ? height - (bounds.y + bounds.h) : 0;
        const uint32_t top    = width  > (bounds.x + bounds.w) ? width - (bounds.x + bounds.w) : 0;
        const uint32_t right  = height - bounds.y;
        const uint32_t bottom = width  - bounds.x;
        // clang-format on

        return { (int)left, (int)top, (int)right, (int)bottom };
    }

    void Framebuffer::setViewport(const Rect& viewport)
    {
        auto result = calculateBounds(viewport, this->width, this->height);

        if (result == Rect::EMPTY)
            result = { 0, 0, this->width, this->height };

        Mtx_OrthoTilt(&this->projection, (float)result.x, (float)this->width, (float)this->height,
                      (float)result.h, Framebuffer::Z_NEAR, Framebuffer::Z_FAR, true);

        this->viewport = result;
    }

    void Framebuffer::setScissor(const Rect& scissor)
    {
        const bool enabled   = scissor != Rect::EMPTY;
        GPU_SCISSORMODE mode = enabled ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

        auto result = calculateBounds(scissor, this->width, this->height);

        if (result == Rect::EMPTY)
            result = { 0, 0, this->width, this->height };

        C3D_SetScissor(mode, result.x, result.y, result.w, result.h);

        this->scissor = result;
    }
} // namespace love
