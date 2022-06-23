#pragma once

#include "modules/graphics/graphics.h"

namespace love::deko3d
{
    class Graphics : public love::Graphics
    {
      public:
        Graphics();

        virtual ~Graphics();

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

        void Line(const Vector2* points, int count) override;

        /* End Primitives */
        BatchedVertexData RequestBatchedDraw(const DrawCommand& command) override;

        void FlushBatchedDraws() override;

        Font* NewDefaultFont(int size, TrueTypeRasterizer::Hinting hinting) override;

        // Internal?
        Shader* NewShader(Shader::StandardShader type);

      private:
        int CalculateEllipsePoints(float rx, float ry) const;

        StreamDrawState streamDrawState;
    };
} // namespace love::deko3d
