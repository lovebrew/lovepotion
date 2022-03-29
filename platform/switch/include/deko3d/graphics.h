#pragma once

#include "modules/graphics/graphics.h"

#include "deko3d/deko.h"

#define RENDERER_NAME    "deko3d"
#define RENDERER_VERSION "0.4.0"
#define RENDERER_VENDOR  "devkitPro"
#define RENDERER_DEVICE  "NVIDIA Tegra X1"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
      public:
        Graphics();

        virtual ~Graphics();

        void Clear(std::optional<Colorf> color, std::optional<int> stencil,
                   std::optional<double> depth) override;

        void Clear(std::vector<std::optional<Colorf>>& colors, std::optional<int> stencil,
                   std::optional<double> depth) override;

        void Present() override;

        void SetScissor(const Rect& scissor) override;

        void SetScissor() override;

        void SetColor(Colorf color) override;

        /* Primitives */

        void Rectangle(DrawMode mode, float x, float y, float width, float height) override;

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry) override;

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry, int points) override;

        void Ellipse(DrawMode mode, float x, float y, float a, float b) override;

        void Ellipse(DrawMode mode, float x, float y, float a, float b, int points) override;

        void Circle(DrawMode mode, float x, float y, float radius) override;

        void Circle(DrawMode mode, float x, float y, float radius, int points) override;

        void Polyline(const Vector2* points, size_t count);

        void Polygon(DrawMode mode, const Vector2* points, size_t size,
                     bool skipLastFilledVertex = true) override;

        void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1,
                 float angle2) override;

        void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1,
                 float angle2, int points) override;

        void Points(const Vector2* points, size_t count, const Colorf* colors,
                    size_t colorCount) override;

        void SetPointSize(float size) override;

        void Line(const Vector2* points, int count) override;

        void SetLineWidth(float width) override;

        void SetDefaultFilter(const Texture::Filter& filter);

        /* End Primitives */

        void SetBlendMode(BlendMode mode, BlendAlpha alpha) override;

        void SetColorMask(ColorMask mask) override;

        void SetMeshCullMode(vertex::CullMode cull) override;

        void SetFrontFaceWinding(vertex::Winding winding) override;

        Font* NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting,
                             const Texture::Filter& filter = Texture::defaultFilter) override;

        Font* NewFont(Rasterizer* rasterizer, const Texture::Filter& filter) override;

        RendererInfo GetRendererInfo() const override;

        // Internal?
        Shader* NewShader(Shader::StandardShader type);

      private:
        int CalculateEllipsePoints(float rx, float ry) const;
    };
} // namespace love::deko3d
