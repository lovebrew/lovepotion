#pragma once

#include "modules/graphics/Graphics.tcc"

namespace love
{
    class Graphics : public GraphicsBase
    {
      public:
        Graphics();

        void backbufferChanged(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                               bool backBufferDepth, int msaa);

        void backbufferChanged(int width, int height, int pixelWidth, int pixelHeight)
        {
            this->backbufferChanged(width, height, pixelWidth, pixelHeight, this->backBufferHasStencil,
                                    this->backBufferHasDepth, this->requestedBackbufferMSAA);
        }

        virtual void clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth) override;

        virtual void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil,
                           OptionalDouble depth) override;

        void present(void* screenshotCallbackData) override;

        void setScissor(const Rect& scissor) override;

        void setScissor() override;

        void setFrontFaceWinding(Winding winding) override;

        void setColorMask(ColorChannelMask mask) override;

        void setBlendState(const BlendState& state) override;

        bool setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                     bool backBufferDepth, int msaa) override;

        bool isActive() const;

        void unsetMode() override;

      private:
        bool backBufferHasStencil;
        bool backBufferHasDepth;
        int requestedBackbufferMSAA;
    };
} // namespace love
