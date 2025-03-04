#include "driver/display/Framebuffer.hpp"

namespace love
{
    Framebuffer::Framebuffer()
    {}

    Framebuffer::~Framebuffer()
    {
        this->destroy();
    }

    void Framebuffer::create(const ScreenInfo& info, dk::Device& device, CMemPool& images, bool depth)
    {
        const auto flags  = depth ? BASE_FLAGS : MAIN_FLAGS;
        const auto format = depth ? DkImageFormat_Z24S8 : DkImageFormat_RGBA8_Unorm;

        dk::ImageLayout layout {};
        dk::ImageLayoutMaker { device }
            .setFlags(flags)
            .setFormat(format)
            .setDimensions(info.width, info.height)
            .initialize(layout);

        this->memory = images.allocate(layout.getSize(), layout.getAlignment());
        this->image.initialize(layout, this->memory.getMemBlock(), this->memory.getOffset());
    }

    void Framebuffer::destroy()
    {
        this->memory.destroy();
    }
} // namespace love
