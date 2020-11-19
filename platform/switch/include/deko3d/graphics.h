#pragma once

#include "modules/graphics/graphics.h"
#include "deko3d/deko.h"

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define RENDERER_NAME    "deko3d"
#define RENDERER_VERSION "0.2.0"
#define RENDERER_VENDOR  "devkitPro"
#define RENDERER_DEVICE  "Tegra X1"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
        public:
            Graphics();

            ~Graphics();

            void Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth) override;

            void Present() override;

            void SetScissor(const Rect & scissor) override;

            void SetScissor() override;

            void SetColor(Colorf color) override;

            /* Primitives */

            void Rectangle(DrawMode mode, float x, float y, float width, float height) override;

            void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry) override;

            void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx, float ry, int points) override;

            void Ellipse(DrawMode mode, float x, float y, float a, float b) override;

            void Ellipse(DrawMode mode, float x, float y, float a, float b, int points) override;

            void Circle(DrawMode mode, float x, float y, float radius) override;

            void Circle(DrawMode mode, float x, float y, float radius, int points) override;

            void Polygon(DrawMode mode, const Vector2 * points, size_t size, bool skipLastFilledVertex = true) override;

            void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2) override;

            void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2, int points) override;

            void Points(const Vector2 * points, size_t count, const Colorf * colors, size_t colorCount) override;

            void SetPointSize(float size) override;

            void Line(float startx, float starty, float endx, float endy) override;

            void SetLineWidth(float width) override;

            void SetDefaultFilter(const Texture::Filter & filter);

            /* End Primitives */

            Image * NewImage(Texture::TextureType type, int width, int height);

            void SetBlendMode(BlendMode mode, BlendAlpha alpha) override;

            void SetColorMask(ColorMask mask) override;

            void SetMeshCullMode(vertex::CullMode cull) override;

            void SetFrontFaceWinding(vertex::Winding winding) override;

            Font * NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting, const Texture::Filter & filter = Texture::defaultFilter) override;

            RendererInfo GetRendererInfo() const override;

            // Internal?
            Shader * NewShader(Shader::StandardShader type);

        private:
            int CalculateEllipsePoints(float rx, float ry) const;
    };
}