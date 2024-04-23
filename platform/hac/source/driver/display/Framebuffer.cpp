#include "driver/display/Framebuffer.hpp"

namespace love
{
    Framebuffer::Framebuffer()
    {}

    Framebuffer::~Framebuffer()
    {}

    void Framebuffer::create(const ScreenInfo& info, dk::Device& device, CMemPool& images,
                             bool depth)
    {
        const auto flags  = depth ? BASE_FLAGS : MAIN_FLAGS;
        const auto format = depth ? DkImageFormat_Z24S8 : DkImageFormat_RGBA8_Unorm;

        dk::ImageLayoutMaker { device }
            .setFlags(flags)
            .setFormat(format)
            .setDimensions(info.width, info.height)
            .initialize(this->layout);

        const auto layoutSize = this->layout.getSize();
        const auto alignment  = this->layout.getAlignment();

        this->memory = images.allocate(layoutSize, alignment);

        const auto& memoryBlock = this->memory.getMemBlock();
        auto memoryOffset       = this->memory.getOffset();

        this->image.initialize(this->layout, memoryBlock, memoryOffset);
    }

    void Framebuffer::destroy()
    {
        this->memory.destroy();
    }
} // namespace love
