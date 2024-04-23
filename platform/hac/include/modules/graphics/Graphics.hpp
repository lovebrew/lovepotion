#pragma once

#include "modules/graphics/Graphics.tcc"

namespace love
{
    class Graphics : public GraphicsBase<Graphics>
    {
      public:
        Graphics();

        void backbufferChanged(int width, int height, int pixelWidth, int pixelHeight,
                               bool backBufferStencil, bool backBufferDepth, int msaa);

        void backbufferChanged(int width, int height, int pixelWidth, int pixelHeight)
        {
            this->backbufferChanged(width, height, pixelWidth, pixelHeight,
                                    this->backBufferHasStencil, this->backBufferHasDepth,
                                    this->requestedBackbufferMSAA);
        }

        void clearImpl(OptionalColor color, OptionalInt depth, OptionalDouble stencil);

        using GraphicsBase<Graphics>::clear;

        void presentImpl();

        void setScissorImpl(const Rect& scissor);

        void setScissorImpl();

        void setFrontFaceWindingImpl(Winding winding);

        void setColorMaskImpl(ColorChannelMask mask);

        void setBlendStateImpl(const BlendState& state);

        void getRendererInfoImpl(RendererInfo& info) const;

        bool setModeImpl(int width, int height, int pixelWidth, int pixelHeight,
                         bool backBufferStencil, bool backBufferDepth, int msaa);

        bool isActive() const;

        void unsetModeImpl();

      private:
        bool backBufferHasStencil;
        bool backBufferHasDepth;
        int requestedBackbufferMSAA;
    };
} // namespace love
