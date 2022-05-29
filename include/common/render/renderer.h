#pragma once

#include "common/colors.h"
#include "common/render/renderstate.h"
#include "common/render/vertex.h"

#include <optional>

namespace love
{
    class Canvas;

    class Renderer
    {
      public:
        /* clear the background to an optional color */
        virtual void Clear(std::optional<Colorf> color) = 0;

        /* clear the depth and stencil buffers */
        virtual void ClearDepthStencil(std::optional<int> stencil, std::optional<double> depth) = 0;

        /* set the foreground rendering color */
        virtual void SetBlendColor(const Colorf& color) = 0;

        /* binds a framebuffer or Canvas as the active buffer */
        virtual void BindFramebuffer(Canvas* canvas) = 0;

        /* presents our framebuffer */
        virtual void Present() = 0;

        /* sets the viewport */
        virtual void SetViewport(const Rect& view) = 0;

        /* sets the scissor region */
        virtual void SetScissor(const Rect& scissor) = 0;

        /* sets mesh culling mode */
        virtual void SetMeshCullMode(Vertex::CullMode mode) = 0;

        /* sets vertex winding */
        virtual void SetVertexWinding(Vertex::Winding winding) = 0;
    };
} // namespace love
