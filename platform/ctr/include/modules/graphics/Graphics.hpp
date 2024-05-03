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

        void clear(OptionalColor color, OptionalInt depth, OptionalDouble stencil);

        void clear(const std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth);

        void present();

        void setScissor(const Rect& scissor);

        void setScissor();

        void setFrontFaceWinding(Winding winding);

        void setColorMask(ColorChannelMask mask);

        void setBlendState(const BlendState& state);

        bool setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                     bool backBufferDepth, int msaa);

        void draw(const DrawIndexedCommand& command) override;

        using GraphicsBase::draw;

        void points(Vector2* positions, const Color* colors, int count);

        bool isActive() const;

        void unsetMode();

        void setViewport(int x, int y, int width, int height);

        // clang-format off
        virtual TextureBase* newTexture(const TextureBase::Settings& settings, const TextureBase::Slices* data = nullptr) override;
        // clang-format on

        bool isPixelFormatSupported(PixelFormat format, uint32_t usage);

        /* TODO: implement properly */
        bool isRenderTargetActive(Texture* texture)
        {
            return false;
        }

        bool is3D() const;

        void set3D(bool enable);

        bool isWide() const;

        void setWide(bool enable);

        float getDepth() const;
    };
} // namespace love
