#pragma once

#include "common/colors.h"
#include "common/render/renderstate.h"
#include "common/render/samplerstate.h"
#include "common/render/vertex.h"

#include <optional>

namespace love
{
    class Canvas;

    /*
    ** Abstract class for renderers
    */
    class Renderer
    {
      public:
        struct RendererInfo
        {
            const char* name;
            const char* version;
            const char* vendor;
            const char* device;
        };

        /* clear the background to an optional color */
        virtual void Clear(std::optional<Colorf> color) = 0;

        /* clear the depth and stencil buffers */
        virtual void ClearDepthStencil(std::optional<int> stencil, std::optional<double> depth) = 0;

        /* set the foreground rendering color */
        virtual void SetBlendColor(const Colorf& color) = 0;

        /* binds a framebuffer or Canvas as the active buffer */
        virtual void BindFramebuffer(Canvas* canvas = nullptr) = 0;

        /* presents our framebuffer */
        virtual void Present() = 0;

        /* sets the viewport */
        virtual void SetViewport(const Rect& view) = 0;

        /* gets the viewport */
        virtual Rect GetViewport() const = 0;

        /* sets the scissor region */
        virtual void SetScissor(const Rect& scissor) = 0;

        /* sets mesh culling mode */
        virtual void SetMeshCullMode(Vertex::CullMode mode) = 0;

        /* sets vertex winding */
        virtual void SetVertexWinding(Vertex::Winding winding) = 0;

        /* gets the renderer info */
        const RendererInfo& GetRendererInfo() const;

        virtual void SetSamplerState(const SamplerState& state) = 0;

      protected:
        virtual void InitRenderInfo() = 0;

        RendererInfo info;
    };
} // namespace love
