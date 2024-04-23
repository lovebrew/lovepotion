#pragma once

#include "common/screen.hpp"

#include "driver/display/Framebuffer.tcc"

#include <coreinit/memfrmheap.h>

#include <gx2/context.h>
#include <gx2/mem.h>
#include <gx2/registers.h>

#include <glm/mat4x4.hpp>

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

        GX2ColorBuffer& get()
        {
            return this->target;
        }

        GX2DepthBuffer& getDepth()
        {
            return this->depth;
        }

        glm::mat4& getModelView()
        {
            return this->modelView;
        }

        glm::mat4& getProjection()
        {
            return this->projection;
        }

        bool allocateScanBuffer(MEMHeapHandle handle);

        bool invalidateColorBuffer(MEMHeapHandle handle);

        bool invalidateDepthBuffer(MEMHeapHandle handle);

        void copyScanBuffer();

      private:
        static constexpr auto FORMAT      = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        static constexpr auto BUFFER_MODE = GX2_BUFFERING_MODE_DOUBLE;
        static constexpr auto INVALIDATE_COLOR_BUFFER =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_COLOR_BUFFER;

        GX2ColorBuffer target;
        GX2DepthBuffer depth;

        uint8_t mode;
        uint8_t id;

        glm::mat4 modelView;
        glm::mat4 projection;

        void* scanBuffer;
        uint32_t scanBufferSize;
    };
} // namespace love
