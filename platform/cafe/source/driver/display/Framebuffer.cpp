#include "driver/display/Framebuffer.hpp"
#include "driver/display/utility.hpp"

#include <gx2/swap.h>

#include <malloc.h>

namespace love
{
    Framebuffer::Framebuffer() : target {}, depth {}, scanBuffer(nullptr), scanBufferSize(0)
    {
        this->uniform            = (Uniform*)malloc(sizeof(Uniform));
        this->uniform->modelView = glm::mat4(1.0f);
    }

    void Framebuffer::destroy()
    {
        std::free(this->uniform);
    }

    bool Framebuffer::allocateScanBuffer(MEMHeapHandle handle)
    {
        const auto alignment = GX2_SCAN_BUFFER_ALIGNMENT;
        this->scanBuffer     = MEMAllocFromFrmHeapEx(handle, this->scanBufferSize, alignment);

        if (this->scanBuffer == nullptr)
            return false;

        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, this->scanBuffer, this->scanBufferSize);

        if (this->id == GX2_SCAN_TARGET_TV)
        {
            const auto mode = (GX2TVRenderMode)this->mode;
            GX2SetTVBuffer(this->scanBuffer, this->scanBufferSize, mode, FORMAT, BUFFER_MODE);
        }
        else
        {
            const auto mode = (GX2DrcRenderMode)this->mode;
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

        GX2Invalidate(GX2_INVALIDATE_MODE_CPU, this->depth.surface.image, size);

        return true;
    }

    void Framebuffer::copyScanBuffer()
    {
        GX2CopyColorBufferToScanBuffer(&this->target, (GX2ScanTarget)this->mode);
    }

    void Framebuffer::create(const ScreenInfo& info)
    {
        uint32_t unknown = 0;

        initColorBuffer(this->target, info.width, info.height);
        initDepthBuffer(this->depth, info.width, info.height);

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

            this->mode = mode;
        }
        else
        {
            GX2SetDRCEnable(true);
            GX2DrcRenderMode mode = GX2_DRC_RENDER_MODE_SINGLE;

            GX2CalcDRCSize(mode, FORMAT, BUFFER_MODE, &this->scanBufferSize, &unknown);
            GX2SetDRCScale(info.width, info.height);

            this->mode = mode;
        }

        this->width  = info.width;
        this->height = info.height;
        this->id     = info.id;

        this->viewport = { 0, 0, info.width, info.height };
        this->scissor  = { 0, 0, info.width, info.height };

        this->uniform->projection =
            glm::ortho(0.0f, (float)info.width, (float)info.height, 0.0f, Z_NEAR, Z_FAR);
    }

    void Framebuffer::setScissor(const Rect& scissor)
    {
        if (scissor == Rect::EMPTY)
            this->scissor = { 0, 0, this->width, this->height };
        else
            this->scissor = scissor;

        GX2SetScissor(this->scissor.x, this->scissor.y, this->scissor.w, this->scissor.h);
    }

    void Framebuffer::setViewport(const Rect& viewport)
    {
        if (viewport == Rect::EMPTY)
            this->viewport = { 0, 0, this->width, this->height };
        else
            this->viewport = viewport;

        GX2SetViewport(this->viewport.x, this->viewport.y, this->viewport.w, this->viewport.h, Z_NEAR, Z_FAR);
    }
} // namespace love
