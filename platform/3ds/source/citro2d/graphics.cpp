#include <citro2d.h>

#include "citro2d/graphics.h"
#include "common/bidirectionalmap.h"

#define TRANSPARENCY C2D_Color32(0, 0, 0, 1)

using namespace love;

inline const auto normalizeAngle = [](float angle) {
    angle = fmodf(angle, LOVE_M_TAU);
    if (angle < 0)
        angle += LOVE_M_TAU;

    return angle;
};

inline std::vector<Vector2> GenerateOutline(const Vector2* points, size_t count, float lineWidth)
{
    std::vector<Vector2> innerPoints(count);

    for (size_t startPoint = 0; startPoint < count; startPoint++)
    {
        const auto& middle = points[startPoint];
        const auto& after  = points[(startPoint + 1) % count];
        const auto& before = points[startPoint == 0 ? count - 1 : startPoint - 1];

        const float theta = normalizeAngle(atan2f(middle.y - after.y, middle.x - after.x));
        const float phi   = normalizeAngle(atan2f(middle.y - before.y, middle.x - before.x));

        const float angleWithinPolygon   = normalizeAngle(phi - theta);
        const float angleOfRightTriangle = (LOVE_M_PI - angleWithinPolygon) / 2;

        const float lengthOfKite = lineWidth * (1 / cosf(angleOfRightTriangle));

        const float offsetX = cosf(theta + (LOVE_M_2_PI - angleOfRightTriangle)) * lengthOfKite;
        const float offsetY = sinf(theta + (LOVE_M_2_PI - angleOfRightTriangle)) * lengthOfKite;

        innerPoints[startPoint] = Vector2(middle.x - offsetX, middle.y - offsetY);
    }

    return innerPoints;
}

namespace love
{
    Renderer& Graphics::Renderer()
    {
        return ::citro2d::Instance();
    }

    namespace citro2d
    {
        void Graphics::Set3D(bool enabled)
        {
            ::citro2d::Instance().Set3D(enabled);
        }

        const bool Graphics::Get3D() const
        {
            return ::citro2d::Instance().Get3D();
        }

        void Graphics::SetWide(bool enabled)
        {
            ::citro2d::Instance().SetWideMode(enabled);
        }

        const bool Graphics::GetWide() const
        {
            return ::citro2d::Instance().GetWide();
        }

        Font* love::citro2d::Graphics::NewDefaultFont(int size)
        {
            auto fontModule = Module::GetInstance<FontModule>(M_FONT);
            if (!fontModule)
                throw love::Exception("Font module has not been loaded.");

            StrongReference<Rasterizer> r(fontModule->NewBCFNTRasterizer(size), Acquire::NORETAIN);

            return new Font(r.Get());
        }

        void Graphics::Points(const Vector2* points, size_t count, const Colorf* colors,
                              size_t colorCount)
        {
            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            for (size_t index = 0; index < count; index++)
            {
                Colorf color        = colors[0];
                const Vector2 point = points[index];

                if (index < colorCount)
                    color = colors[index];

                u32 pointColor = C2D_Color32f(color.r, color.g, color.b, color.a);
                C2D_DrawCircleSolid(point.x, point.y, Graphics::CURRENT_DEPTH,
                                    this->states.back().pointSize, pointColor);
            }
        }

        void Graphics::Polyfill(const Vector2* points, size_t count, u32 color, float depth)
        {
            for (size_t currentPoint = 2; currentPoint < count; currentPoint++)
            {
                C2D_DrawTriangle(points[0].x, points[0].y, color, points[currentPoint - 1].x,
                                 points[currentPoint - 1].y, color, points[currentPoint].x,
                                 points[currentPoint].y, color, depth);
            }
        }

        void Graphics::Polygon(DrawMode mode, const Vector2* points, size_t count)
        {
            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            if (mode == DRAW_LINE)
                this->Polyline(points, count);
            else
                this->Polyfill(points, count, foreground, Graphics::CURRENT_DEPTH);
        }

        void Graphics::Polyline(const Vector2* points, size_t count)
        {
            // Generate the outline and draw it
            std::vector<Vector2> outline =
                GenerateOutline(points, count, this->states.back().lineWidth);
            this->Polyfill(outline.data(), outline.size(), TRANSPARENCY,
                           Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);

            // Draw our filled polygon
            this->Polygon(DRAW_FILL, points, count);

            Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
        }

        void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height)
        {
            Vector2 points[4] = {
                { x, y }, { x + width, y }, { x + width, y + height }, { x, y + height }
            };

            this->Polygon(mode, points, 4);
        }

        void Graphics::Rectangle(DrawMode mode, float x, float y, float width, float height,
                                 float rx, float ry)
        {
            if (rx == 0 && ry == 0)
            {
                this->Rectangle(mode, x, y, width, height);
                return;
            }

            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            /* Offset the radii *properly* */
            Vector2 offset(x + rx, y + ry);
            Vector2 size(rx * 2, ry * 2);

            /*
            ** Ellipse Drawing Order
            ** 1 - 4
            ** |   |
            ** 2 - 3
            */

            if (mode == DRAW_FILL)
            {
                /* Draw Ellipses first on Fill mode */

                C2D_DrawEllipseSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2,
                                     size.x, size.y, foreground);

                C2D_DrawEllipseSolid(x, y + (height - size.y),
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                C2D_DrawEllipseSolid(x + (width - size.x), y + (height - size.y),
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                C2D_DrawEllipseSolid(x + (width - size.x), y,
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                /* Draw Rectangles */

                C2D_DrawRectSolid(offset.x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                                  width - size.x, height, foreground);

                C2D_DrawRectSolid(x, offset.y, Graphics::CURRENT_DEPTH, width, height - size.y,
                                  foreground);

                Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH * 2;
            }
            else
            {
                float& lineWidth = this->states.back().lineWidth;

                Vector2 innerDiameter((rx - lineWidth) * 2, (ry - lineWidth) * 2);
                if (innerDiameter.x <= 0 || innerDiameter.y <= 0)
                {
                    innerDiameter.x = 0;
                    innerDiameter.y = 0;
                }

                /* normal rect offset + line width */
                Vector2 lineOffset((x + rx) + lineWidth, (y + ry) + lineWidth);

                /* normal radii size - line width */
                Vector2 lineSize((rx * 2) - lineWidth, (ry * 2) - lineWidth);

                /* normal radii pos + line width */
                Vector2 linePos(x + lineWidth, y + lineWidth);

                /* normal rect size - line width */
                Vector2 rectSize(width - (lineWidth * 2), height - (lineWidth * 2));

                /* Transparent rectangles first */

                C2D_DrawRectSolid(x + innerDiameter.x / 2 + lineWidth, y + lineWidth,
                                  Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                  width - (lineWidth * 2 + innerDiameter.x), height - lineWidth * 2,
                                  TRANSPARENCY);

                C2D_DrawRectSolid(x + lineWidth, y + innerDiameter.y / 2 + lineWidth,
                                  Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                  width - (lineWidth * 2),
                                  height - (lineWidth * 2 + innerDiameter.y), TRANSPARENCY);

                /* Transparent ellipses second, if they aren't nonexistent */

                if (innerDiameter.x > 0 && innerDiameter.y > 0)
                {
                    C2D_DrawEllipseSolid(x + lineWidth, y + lineWidth,
                                         Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                         innerDiameter.x, innerDiameter.y, TRANSPARENCY);

                    C2D_DrawEllipseSolid(x + lineWidth, y + height - ry - innerDiameter.y / 2,
                                         Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                         innerDiameter.x, innerDiameter.y, TRANSPARENCY);

                    C2D_DrawEllipseSolid(x + width - rx - innerDiameter.x / 2,
                                         y + height - ry - innerDiameter.y / 2,
                                         Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                         innerDiameter.x, innerDiameter.y, TRANSPARENCY);

                    C2D_DrawEllipseSolid(x + width - rx - innerDiameter.x / 2, y + lineWidth,
                                         Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 3,
                                         innerDiameter.x, innerDiameter.y, TRANSPARENCY);
                }

                /* Solid stuff  -- Start with ellipses */

                C2D_DrawEllipseSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2,
                                     size.x, size.y, foreground);

                C2D_DrawEllipseSolid(x, y + (height - size.y),
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                C2D_DrawEllipseSolid(x + (width - size.x), y + (height - size.y),
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                C2D_DrawEllipseSolid(x + (width - size.x), y,
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH * 2, size.x,
                                     size.y, foreground);

                /* Rectangles */

                C2D_DrawRectSolid(offset.x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                                  width - size.x, height, foreground);

                C2D_DrawRectSolid(x, offset.y, Graphics::CURRENT_DEPTH, width, height - size.y,
                                  foreground);

                /* Ellipses */

                Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH * 3;
            }
        }

        void Graphics::Ellipse(DrawMode mode, float x, float y, float a, float b)
        {
            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            if (mode == DRAW_FILL)
                C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2,
                                     foreground);
            else
            {
                float lineWidth = this->states.back().lineWidth;

                C2D_DrawEllipseSolid((x - a) + lineWidth, (y - b) + lineWidth,
                                     Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                                     (a - lineWidth) * 2, (b - lineWidth) * 2, TRANSPARENCY);

                C2D_DrawEllipseSolid(x - a, y - b, Graphics::CURRENT_DEPTH, a * 2, b * 2,
                                     foreground);

                Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
            }
        }

        void Graphics::Circle(DrawMode mode, float x, float y, float radius)
        {
            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            if (mode == DRAW_FILL)
                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);
            else
            {
                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                                    radius - this->states.back().lineWidth, TRANSPARENCY);

                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);

                Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
            }
        }

        void Graphics::Arc(DrawMode mode, ArcMode arcmode, float x, float y, float radius,
                           float angle1, float angle2)
        {
            const float diag_radius = LOVE_M_SQRT2 * radius;
            const auto calc90Triangle =
                [radius = diag_radius](float x, float y, float angle) -> std::array<Vector2, 3> {
                return { Vector2(x, y), Vector2(x + radius * cosf(angle), y + radius * sinf(angle)),
                         Vector2(x + radius * sqrtf(2) * cosf(angle + LOVE_M_PI_2),
                                 y + radius * sqrtf(2) * sinf(angle + LOVE_M_PI_2)) };
            };

            angle1 = normalizeAngle(angle1);
            angle2 = normalizeAngle(angle2);

            // Only go around counterclockwise rather than having a conditional
            if (angle2 > angle1)
                angle2 -= LOVE_M_TAU;

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            while (angle2 + LOVE_M_PI_2 < angle1)
            {
                const auto& pts = calc90Triangle(x, y, angle2);
                C2D_DrawTriangle(pts[0].x, pts[0].y, TRANSPARENCY, pts[1].x, pts[1].y, TRANSPARENCY,
                                 pts[2].x, pts[2].y, TRANSPARENCY,
                                 Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);
                angle2 += LOVE_M_PI_2;
            }

            const std::array<Vector2, 3> finalTriangle = {
                Vector2(x, y),
                Vector2(x + diag_radius * cosf(angle2), y + diag_radius * sinf(angle2)),
                Vector2(x + diag_radius * cosf(angle1), y + diag_radius * sinf(angle1))
            };

            C2D_DrawTriangle(finalTriangle[0].x, finalTriangle[0].y, TRANSPARENCY,
                             finalTriangle[1].x, finalTriangle[1].y, TRANSPARENCY,
                             finalTriangle[2].x, finalTriangle[2].y, TRANSPARENCY,
                             Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH);

            /* Sort of code duplication, but uh.. fix the arcs! */

            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            if (mode == DRAW_FILL)
                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);
            else
            {
                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH + Graphics::MIN_DEPTH,
                                    radius - this->states.back().lineWidth, TRANSPARENCY);

                C2D_DrawCircleSolid(x, y, Graphics::CURRENT_DEPTH, radius, foreground);

                Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
            }

            Graphics::CURRENT_DEPTH += Graphics::MIN_DEPTH;
        }

        void Graphics::Line(const Vector2* points, int count)
        {
            Colorf color   = this->GetColor();
            u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

            const Matrix4& t = this->GetTransform();
            C2D_ViewRestore(&t.GetElements());

            for (size_t index = 1; index < (size_t)count; index++)
                C2D_DrawLine(points[index - 1].x, points[index - 1].y, foreground, points[index].x,
                             points[index].y, foreground, this->states.back().lineWidth,
                             Graphics::CURRENT_DEPTH);
        }
    } // namespace citro2d
} // namespace love
