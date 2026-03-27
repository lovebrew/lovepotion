#include <algorithm>

#include "common/Exception.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/citro3d.hpp"

namespace love
{
    Framebuffer::Framebuffer() : target(nullptr)
    {}

    void Framebuffer::create(const ScreenInfo& info)
    {
        const auto side = (gfxIs3D() && info.name == "right") ? GFX_RIGHT : GFX_LEFT;

        this->target = C3D_RenderTargetCreate(info.height, info.width, COLOR_FORMAT, DEPTH_FORMAT);

        if (!this->target)
            throw love::Exception("Failed to create {:s} render target.", info.name);

        const auto screen = (gfxScreen_t)info.id;
        C3D_RenderTargetSetOutput(this->target, screen, side, DISPLAY_TRANSFER_FLAGS);

        this->width  = info.width;
        this->height = info.height;

        this->viewport = { 0, 0, this->width, this->height };
        this->scissor  = { 0, 0, this->width, this->height };
    }

    void Framebuffer::destroy()
    {
        if (!this->target)
            return;

        C3D_RenderTargetDelete(this->target);
        this->target = nullptr;
    }

} // namespace love
