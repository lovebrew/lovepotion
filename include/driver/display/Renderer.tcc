#pragma once

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/renderstate.hpp"

#include "modules/graphics/vertex.hpp"
#include <cstdint>

namespace love
{
    class RendererBase
    {
      public:
        RendererBase()
        {}

        virtual void init() = 0;

        virtual void close() = 0;

        virtual void clearColor(const Color& color) = 0;

        virtual void clear(double depth, int stencil) = 0;

        virtual void setClearDepth(double depth) = 0;

        virtual void setClearStencil(int stencil) = 0;

        virtual void setStencilState(const StencilState& state) = 0;

        virtual void setViewport(const Rect& viewport) = 0;

        virtual void setScissor(const Rect& scissor) = 0;

        virtual void setCullMode(CullMode mode) = 0;

        virtual void setVertexAttributes(const VertexAttributes& attributes,
                                         const BufferBindings& buffers) = 0;

        virtual void prepareDraw(GraphicsBase* graphics) = 0;

        virtual void present() = 0;

        bool isInFrame() const
        {
            return this->inFrame;
        }

      protected:
        struct ContextBase
        {
            CullMode cullMode;
            ColorChannelMask colorMask;
            BlendState blendState;
            StencilState stencilState;

            Rect scissor;
            Rect viewport;

            uint8_t currentTextureUnit;
            bool depthWrites;
        };

        struct State
        {
            uint32_t enabledAttribArrays;
        } renderState;

        bool initialized = false;
        bool inFrame     = false;
    };
} // namespace love
