#pragma once

#include "common/screen.hpp"

#include "driver/display/Framebuffer.tcc"

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

        GX2ColorBuffer& get()
        {
            return this->target;
        }

        GX2DepthBuffer& getDepth()
        {
            return this->depth;
        }

        bool allocateScanBuffer(MEMHeapHandle handle);

        bool invalidateColorBuffer(MEMHeapHandle handle);

        bool invalidateDepthBuffer(MEMHeapHandle handle);

        void copyScanBuffer();

        void setState()
        {
            GX2SetContextState(this->state);
        }

      private:
        static constexpr auto FORMAT      = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        static constexpr auto BUFFER_MODE = GX2_BUFFERING_MODE_DOUBLE;
        static constexpr auto INVALIDATE_COLOR_BUFFER =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_COLOR_BUFFER;
        static constexpr auto INVALIDATE_DEPTH_BUFFER =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_DEPTH_BUFFER;

        GX2ColorBuffer target;
        GX2DepthBuffer depth;

        uint8_t renderMode;
        GX2ScanTarget id;

        GX2ContextState* state;

        void* scanBuffer;
        uint32_t scanBufferSize;
    };
} // namespace love
