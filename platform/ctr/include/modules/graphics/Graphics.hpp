#pragma once

#include "modules/graphics/Graphics.tcc"
#include "modules/graphics/Texture.hpp"

namespace love
{
    class Graphics : public GraphicsBase
    {
      public:
        Graphics();

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

        void draw(Drawable* drawable, const Matrix4& matrix);

        void draw(TextureBase* texture, Quad* quad, const Matrix4& matrix);

        virtual void draw(const DrawCommand& command) override;

        virtual void draw(const DrawIndexedCommand& command) override;

        bool isActive() const;

        void unsetMode();

        void setViewport(int x, int y, int width, int height);

        Texture* newTexture(const Texture::Settings& settings, const Texture::Slices* data = nullptr);

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
