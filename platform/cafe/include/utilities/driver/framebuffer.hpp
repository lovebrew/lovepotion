#pragma once

#include <common/vector.hpp>

#include <coreinit/memfrmheap.h>

#include <gx2/mem.h>
#include <gx2/registers.h>

#include <glm/mat4x4.hpp>

#include <functional>

namespace love
{
    class Framebuffer
    {
      public:
        struct Transform
        {
            glm::mat4 modelView;
            glm::mat4 projection;
        };

        static constexpr size_t TRANSFORM_SIZE = sizeof(Transform);

        Framebuffer();

        ~Framebuffer();

        void Create(Screen id);

        int GetWidth() const
        {
            return this->width;
        }

        int GetHeight() const
        {
            return this->height;
        }

        Vector2 GetSize() const
        {
            return { (float)this->width, (float)this->height };
        }

        GX2ColorBuffer& GetBuffer()
        {
            return this->colorBuffer;
        }

        GX2DepthBuffer& GetDepthBuffer()
        {
            return this->depthBuffer;
        }

        /*
        ** Sets the projection matrix to the specified projection 4x4
        ** Then it converts the main Transform data to little endian
        */
        void SetProjection(const glm::highp_mat4& projection);

        /*
        ** Invalidates the little endian Transform
        ** to be used in the Uniform Block
        */
        void UseProjection();

        /*
        ** Copy our GX2ColorBuffer to the Scan Target
        ** This is used in Renderer::Present
        */
        void CopyScanBuffer();

        operator GX2ColorBuffer*()
        {
            return &this->colorBuffer;
        }

        operator GX2DepthBuffer*()
        {
            return &this->depthBuffer;
        }

        /* Called on Renderer::OnForegroundAcquired */
        bool AllocateScanBuffer(MEMHeapHandle handle);

        /* Called on Renderer::OnForegroundAcquired */
        bool InvalidateColorBuffer(MEMHeapHandle handle);

        /* Called on Renderer::OnForegroundAcquired */
        bool InvalidateDepthBuffer(MEMHeapHandle handle);

      private:
        static constexpr GX2ScanTarget SCAN_TARGETS[0x02] { GX2_SCAN_TARGET_TV,
                                                            GX2_SCAN_TARGET_DRC };

        /* Called on Create */
        void ScanSystemMode();

        /* Called by ScanSystemMode */
        void SetSize(int width, int height);

        /* Called by SetSize when id is TV */
        void SetTVSize();

        /* Called by SetSize when id is Gamepad */
        void SetDRCSize();

        /* Called by SetSize at the end */
        void InitColorBuffer();

        /* Called by SetSize at the end */
        void InitDepthBuffer();

        /* Called by AllocateScanBuffer */
        void SetTVScanBuffer();

        /* Called by AllocateScanBuffer */
        void SetDRCScanBuffer();

        bool Is(Screen id)
        {
            return this->id == id;
        }

        static constexpr auto FORMAT    = GX2_SURFACE_FORMAT_UNORM_R8_G8_B8_A8;
        static constexpr auto BUFFERING = GX2_BUFFERING_MODE_DOUBLE;
        static constexpr auto INVALIDATE_UNIFORM =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_UNIFORM_BLOCK;
        static constexpr auto INVALIDATE_COLOR_BUFFER =
            GX2_INVALIDATE_MODE_CPU | GX2_INVALIDATE_MODE_COLOR_BUFFER;

        Screen id;

        glm::mat4 modelView;
        Transform* transform;

        GX2ColorBuffer colorBuffer;
        GX2DepthBuffer depthBuffer;

        uint8_t mode;

        void* scanBuffer;
        uint32_t scanBufferSize;

        int width;
        int height;
    };
} // namespace love
