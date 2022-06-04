#pragma once

#include "citro2d/citro.h"
#include "modules/graphics/graphics.h"

#include <vector>

namespace love::citro2d
{
    class Graphics : public love::Graphics
    {
      public:
        /* Primitives */

        void Polygon(DrawMode mode, const Vector2* points, size_t count) override;

        void Polyfill(const Vector2* points, size_t count, u32 color, float depth);

        void Polyline(const Vector2* points, size_t count);

        void Rectangle(DrawMode mode, float x, float y, float width, float height) override;

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry);

        void Rectangle(DrawMode mode, float x, float y, float width, float height, float rx,
                       float ry, int points) override
        {
            this->Rectangle(mode, x, y, width, height, rx, ry);
        };

        void Ellipse(DrawMode mode, float x, float y, float a, float b) override;

        void Ellipse(DrawMode mode, float x, float y, float a, float b, int points) override
        {
            this->Ellipse(mode, x, y, a, b);
        };

        void Circle(DrawMode mode, float x, float y, float radius) override;

        void Circle(DrawMode mode, float x, float y, float radius, int points) override {};

        void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1,
                 float angle2);

        void Arc(DrawMode drawmode, ArcMode arcmode, float x, float y, float radius, float angle1,
                 float angle2, int points) {};

        void Points(const Vector2* points, size_t count, const Colorf* colors,
                    size_t colorCount) override;

        void Line(const Vector2* points, int count) override;

        /* End Primitives */

        virtual Font* NewDefaultFont(int size);

        /* Nintendo 3DS */

        void Set3D(bool enable);

        const bool Get3D() const;

        void SetWide(bool enable);

        const bool GetWide() const;

        /* End Nintendo 3DS */
    };
} // namespace love::citro2d
