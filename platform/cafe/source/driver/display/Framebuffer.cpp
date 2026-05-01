#include "common/Exception.hpp"

#include "driver/display/Framebuffer.hpp"
#include "driver/display/utility.hpp"

#include <gx2/swap.h>

#include <malloc.h>

namespace love
{
    Framebuffer::Framebuffer() : target {}, depth {}, scanBuffer(nullptr), scanBufferSize(0)
    {}

    void Framebuffer::destroy()
    {}

    bool Framebuffer::allocateScanBuffer(MEMHeapHandle handle)
    {
        const auto alignment = GX2_SCAN_BUFFER_ALIGNMENT;
        this->scanBuffer     = MEMAllocFromFrmHeapEx(handle, this->scanBufferSize, alignment);

        if (this->scanBuffer == nullptr)
            return false;

        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, this->scanBuffer, this->scanBufferSize);

        if (this->id == GX2_SCAN_TARGET_TV)
        {
            const auto mode = (GX2TVRenderMode)this->renderMode;
            GX2SetTVBuffer(this->scanBuffer, this->scanBufferSize, mode, FORMAT, BUFFER_MODE);
        }
        else
        {
            const auto mode = (GX2DrcRenderMode)this->renderMode;
            GX2SetDRCBuffer(this->scanBuffer, this->scanBufferSize, mode, FORMAT, BUFFER_MODE);
        }

        return true;
    }

    bool Framebuffer::invalidateColorBuffer(MEMHeapHandle handle)
    {
        const auto size      = this->target.surface.imageSize;
        const auto alignment = this->target.surface.alignment;

        this->target.surface.image = MEMAllocFromFrmHeapEx(handle, size, alignment);

        if (this->target.surface.image == nullptr)
            return false;

        GX2Invalidate(INVALIDATE_COLOR_BUFFER, this->target.surface.image, size);

        return true;
    }

    bool Framebuffer::invalidateDepthBuffer(MEMHeapHandle handle)
    {
        const auto size      = this->depth.surface.imageSize;
        const auto alignment = this->depth.surface.alignment;

        this->depth.surface.image = MEMAllocFromFrmHeapEx(handle, size, alignment);

        if (this->depth.surface.image == nullptr)
            return false;

        GX2Invalidate(INVALIDATE_DEPTH_BUFFER, this->depth.surface.image, size);

        return true;
    }

    void Framebuffer::copyScanBuffer()
    {
        GX2CopyColorBufferToScanBuffer(&this->target, this->id);
    }

    void Framebuffer::create(const ScreenInfo& info)
    {
        uint32_t unknown = 0;

        initColorBuffer(this->target, info.width, info.height);
        initDepthBuffer(this->depth, info.width, info.height);

        this->state = (GX2ContextState*)memalign(GX2_CONTEXT_STATE_ALIGNMENT, sizeof(GX2ContextState));

        if (!this->state)
            throw love::Exception("Failed to allocate GX2 context state.");

        GX2SetupContextStateEx(this->state, true);
        GX2SetContextState(this->state);

        if (info.id == GX2_SCAN_TARGET_TV)
        {
            GX2SetTVEnable(true);
            GX2TVRenderMode mode = GX2_TV_RENDER_MODE_WIDE_480P;

            if (info.height == 720)
                mode = GX2_TV_RENDER_MODE_WIDE_720P;
            else if (info.height == 1080)
                mode = GX2_TV_RENDER_MODE_WIDE_1080P;

            GX2CalcTVSize(mode, FORMAT, BUFFER_MODE, &this->scanBufferSize, &unknown);
            GX2SetTVScale(info.width, info.height);
            this->renderMode = mode;
        }
        else
        {
            GX2SetDRCEnable(true);
            GX2DrcRenderMode mode = GX2_DRC_RENDER_MODE_SINGLE;

            GX2CalcDRCSize(mode, FORMAT, BUFFER_MODE, &this->scanBufferSize, &unknown);
            GX2SetDRCScale(info.width, info.height);
            this->renderMode = mode;
        }

        this->id     = (GX2ScanTarget)info.id;
        this->width  = info.width;
        this->height = info.height;

        this->viewport = { 0, 0, info.width, info.height };
        this->scissor  = { 0, 0, info.width, info.height };
    }
} // namespace love
