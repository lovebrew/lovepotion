#pragma once

#include "common/screen.hpp"

#include "driver/display/Framebuffer.tcc"
#include "driver/display/deko3d/CMemPool.h"

#include <deko3d.hpp>

namespace love
{
    class Framebuffer : public FramebufferBase
    {
      public:
        Framebuffer();

        ~Framebuffer();

        void create(const ScreenInfo& info, dk::Device& device, CMemPool& pool, bool depth);

        void destroy();

        dk::Image* getImage()
        {
            return std::addressof(this->image);
        }

      private:
        static constexpr auto BASE_FLAGS = DkImageFlags_UsageRender | DkImageFlags_HwCompression;
        static constexpr auto MAIN_FLAGS = BASE_FLAGS | DkImageFlags_UsagePresent;

        dk::Image image;
        CMemPool::Handle memory;
        dk::ImageLayout layout;
    };
} // namespace love
