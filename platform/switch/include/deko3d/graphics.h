#pragma once

#include "modules/graphics/graphics.h"
#include "deko3d/deko.h"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
        public:
            Graphics();

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

            void Polygon(DrawMode mode, const vertex::Vertex * points, size_t size, bool skipLastFilledVertex = true) override;

            void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2) override;

            void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1, float angle2, int points) override;

            /* End Primitives */

            void SetBlendMode(BlendMode mode, BlendAlpha alpha) override;

            void SetColorMask(ColorMask mask) override;

            void SetMeshCullMode(vertex::CullMode cull) override;

            void SetFrontFaceWinding(vertex::Winding winding) override;

            // Internal?
            Shader * NewShader(void * vertex, size_t vertex_sz, void * pixel, size_t pixel_sz);

            ShaderStage * NewShaderStage(ShaderStage::StageType stage, void * data, size_t size);

        private:
            int CalculateEllipsePoints(float rx, float ry) const;
    };
}