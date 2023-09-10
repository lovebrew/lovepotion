#pragma once

#include <modules/graphics/graphics.tcc>

#include <objects/rasterizer_ext.hpp>

namespace love
{
    template<>
    class Graphics<Console::HAC> : public Graphics<Console::ALL>
    {
      public:
        Graphics();

        virtual ~Graphics();

        bool SetMode(int x, int y, int width, int height);

        void CheckSetDefaultFont();

        void SetShader();

        void SetShader(Shader<Console::HAC>* shader);

        Font* NewFont(Rasterizer<Console::HAC>* data) const;

        Font* NewDefaultFont(int size, Rasterizer<Console::HAC>::Hinting hinting) const;

        Texture<Console::HAC>* NewTexture(const Texture<>::Settings& settings,
                                          const Texture<>::Slices* slices = nullptr) const;

        void Draw(Texture<Console::HAC>* texture, Quad* quad, const Matrix4<Console::HAC>& matrix);

        void Draw(Drawable* drawable, const Matrix4<Console::HAC>& matrix);

        void Print(const Font::ColoredStrings& strings, const Matrix4<Console::HAC>& matrix);

        void Print(const Font::ColoredStrings& strings, Font* font,
                   const Matrix4<Console::HAC>& matrix);

        void Printf(const Font::ColoredStrings& strings, float wrap, Font::AlignMode align,
                    const Matrix4<Console::HAC>& matrix);

        void Printf(const Font::ColoredStrings& strings, Font* font, float wrap,
                    Font::AlignMode align, const Matrix4<Console::HAC>& matrix);

        void SetViewportSize(int width, int height);
    }; // namespace love
} // namespace love
