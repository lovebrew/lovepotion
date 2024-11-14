#pragma once

#include "common/screen.hpp"

#include "driver/display/Framebuffer.tcc"
#include "driver/display/Uniform.hpp"

#include <coreinit/memfrmheap.h>

#include <gx2/context.h>
#include <gx2/mem.h>
#include <gx2/registers.h>

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

        Uniform* getUniform()
        {
            return this->uniform;
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

        uint8_t renderMode;
        GX2ScanTarget id;

        Uniform* uniform = nullptr;

        glm::mat4 tmpModel;
        glm::highp_mat4 ortho;

        void* scanBuffer;
        uint32_t scanBufferSize;
    };
} // namespace love
