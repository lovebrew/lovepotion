#pragma once

#include "common/bidirectionalmap.h"
#include "common/colors.h"

#include "common/render/renderstate.h"
#include "common/render/samplerstate.h"
#include "common/render/vertex.h"

#include "modules/graphics/graphics.h"

#include <optional>

namespace love
{
    class Canvas;
    class Texture;

    /* Abstract class for renderers */
    class Renderer
    {
      public:
        static int shaderSwitches;
        static int drawCalls;

        struct RendererInfo
        {
            const char* name;
            const char* version;
            const char* vendor;
            const char* device;
        };

        /* destroy all framebuffer resources */
        virtual void DestroyFramebuffers() = 0;

        /* create all framebuffer resources */
        virtual void CreateFramebuffers() = 0;

        /* clear the background to an optional color */
        virtual void Clear(const Colorf& color) = 0;

        /* clear the depth and stencil buffers */
        virtual void ClearDepthStencil(int stencil, uint8_t mask, double depth) = 0;

        /* set the foreground rendering color */
        virtual void SetBlendColor(const Colorf& color) = 0;

        virtual void SetBlendMode(const RenderState::BlendState& blend) = 0;

        /* binds a framebuffer or Canvas as the active buffer */
        virtual void BindFramebuffer(Canvas* canvas = nullptr) = 0;

        /* presents our framebuffer */
        virtual void Present() = 0;

        /* sets the viewport */
        virtual void SetViewport(const Rect& viewport) = 0;

        /* gets the viewport */
        virtual Rect GetViewport() const = 0;

        /* sets the scissor region */
        virtual void SetScissor(bool enable, const Rect& scissor, bool canvasActive) = 0;

        /* sets the stencil */
        virtual void SetStencil(RenderState::CompareMode mode, int value) = 0;

        /* sets mesh culling mode */
        virtual void SetMeshCullMode(Vertex::CullMode mode) = 0;

        /* sets vertex winding */
        virtual void SetVertexWinding(Vertex::Winding winding) = 0;

        /* sets the sampler state */
        virtual void SetSamplerState(Texture* texture, SamplerState& state) = 0;

        /* sets the color mask for writing */
        virtual void SetColorMask(const RenderState::ColorMask& mask) = 0;

        /* sets the line width */
        virtual void SetLineWidth(float lineWidth) = 0;

        /* sets the line join style */
        virtual void SetLineStyle(Graphics::LineStyle style) = 0;

        /* sets the point size */
        virtual void SetPointSize(float pointSize) = 0;

        /* gets the renderer info */
        const RendererInfo& GetRendererInfo() const
        {
            return this->rendererInfo;
        };

      protected:
        virtual void InitRendererInfo() = 0;

        RendererInfo rendererInfo;
        bool inFrame = false;

        Rect viewport;
    };
} // namespace love
