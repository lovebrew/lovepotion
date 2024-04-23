#pragma once

#include "modules/graphics/Graphics.tcc"

namespace love
{
    class Graphics : public GraphicsBase<Graphics>
    {
      public:
        Graphics();

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

        void setViewport(int x, int y, int width, int height);

        bool is3D() const;

        void set3D(bool enable);

        bool isWide() const;

        void setWide(bool enable);

        float getDepth() const;
    };
} // namespace love
