#pragma once

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Texture.hpp"

namespace love
{
    class Graphics : public GraphicsBase
    {
      public:
        Graphics();

        ~Graphics();

        virtual void initCapabilities() override;

        virtual void clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth) override;

        virtual void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil,
                           OptionalDouble depth) override;

        virtual void present(void* screenshotCallbackData) override;

        virtual void setScissor(const Rect& scissor) override;

        virtual void setScissor() override;

        virtual void setFrontFaceWinding(Winding winding) override;

        virtual void setColorMask(ColorChannelMask mask) override;

        virtual void setBlendState(const BlendState& state) override;

        virtual bool setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                             bool backBufferDepth, int msaa) override;

        virtual void setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                              bool hasSRGBTexture) override;

        void draw(const DrawIndexedCommand& command) override;

        void draw(const DrawCommand& command) override;

        using GraphicsBase::draw;

        void points(Vector2* positions, const Color* colors, int count);

        bool isActive() const;

        virtual void unsetMode() override;

        void setActiveScreen() override;

        void setViewport(int x, int y, int width, int height);

        C3D_RenderTarget* getInternalBackbuffer() const;

        // clang-format off
        virtual TextureBase* newTexture(const TextureBase::Settings& settings, const TextureBase::Slices* data = nullptr) override;
        // clang-format on

        bool isPixelFormatSupported(PixelFormat format, uint32_t usage);

        bool is3D() const;

        void set3D(bool enable);

        bool isWide() const;

        void setWide(bool enable);

        float getDepth() const;
    };
} // namespace love
