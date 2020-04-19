#include "common/runtime.h"
#include "modules/graphics/graphics.h"
#include "modules/window/window.h"

#include "common/backend/primitives.h"

using namespace love;

#define CUR_DEPTH Graphics::CURRENT_DEPTH
#define MIN_DEPTH Graphics::MIN_DEPTH
#define TRANSPARENCY C2D_Color32f(0, 0, 0, 0)

void Primitives::Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry, float lineWidth, const Color & color)
{
    if (mode == "fill")
        C2D_DrawRectSolid(x, y, CUR_DEPTH, width, height, C2D_Color32f(color.r, color.g, color.b, color.a));
    else if (mode == "line")
    {
        C2D_DrawRectSolid(x + lineWidth, y + lineWidth, CUR_DEPTH + MIN_DEPTH, width - (lineWidth * 2), height - (lineWidth * 2), TRANSPARENCY);
        C2D_DrawRectSolid(x, y, CUR_DEPTH, width, height, C2D_Color32f(color.r, color.g, color.b, color.a));

        CUR_DEPTH += MIN_DEPTH;
    }
}

void Primitives::Circle(const std::string & mode, float x, float y, float radius, float lineWidth, const Color & color)
{
    auto gModule = love::Module::GetInstance<love::Graphics>(Module::M_GRAPHICS);
    Color background = gModule->GetBackgroundColor();

    if (mode == "fill")
        C2D_DrawCircleSolid(x, y, CUR_DEPTH, radius, C2D_Color32f(color.r, color.g, color.b, color.a));
    else if (mode == "line")
    {
        if (radius >= 1.3)
            C2D_DrawCircleSolid(x, y, CUR_DEPTH + MIN_DEPTH, radius - ((gModule->GetLineWidth() * 2 / radius) * radius), C2D_Color32f(background.r, background.g, background.b, background.a));

        C2D_DrawCircleSolid(x, y, CUR_DEPTH, radius, C2D_Color32f(color.r, color.g, color.b, color.a));

        CUR_DEPTH += MIN_DEPTH;
    }
}

void Primitives::Line(float x1, float y1, float x2, float y2, float lineWidth, const Color & color)
{
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);
    C2D_DrawLine(x1, y1, foreground, x2, y2, foreground, lineWidth, CUR_DEPTH);
}

void Primitives::Polygon(const std::string & mode, std::vector<Graphics::Point> points, float lineWidth, const Color & color)
{
    u32 foreground = C2D_Color32f(color.r, color.g, color.b, color.a);

    if (mode == "fill")
    {
        for (size_t currentPoint = 2; currentPoint < points.size(); currentPoint++)
        {
            C2D_DrawTriangle(points[0].x, points[0].y, foreground, points[currentPoint - 1].x, points[currentPoint - 1].y, foreground, points[currentPoint].x,
                points[currentPoint].y, foreground, CUR_DEPTH);
        }
    }
    else
    {
        if (points.size() > 2)
        {
            for (size_t startPoint = 0; startPoint < points.size(); startPoint++)
            {
                const auto & a = points[startPoint];
                const auto & b = points[(startPoint + 1) % points.size()];
                const auto & d = points[startPoint == 0 ? points.size() - 1 : startPoint - 1];
                const auto & c = points[(startPoint + 2) % points.size()];

                // Line segment angles
                float daAngle = atan2f(d.y - a.y, d.x - a.x);
                if (daAngle < 0)
                    daAngle = C3D_Angle(1) + daAngle;

                float abAngle = atan2f(a.y - b.y, a.x - b.x);
                if (abAngle < 0)
                    abAngle = C3D_Angle(1) + abAngle;

                float baAngle = atan2f(b.y - a.y, b.x - a.x);
                if (baAngle < 0)
                    baAngle = C3D_Angle(1) + baAngle;

                float bcAngle = atan2f(c.y - b.y, c.x - b.x);
                if (bcAngle < 0)
                    bcAngle = C3D_Angle(1) + bcAngle;

                // NOTE: have to handle differing signs properly

                // Angle of line segment DA with respect to AB
                float dabTheta = daAngle + baAngle;
                float dabPhi   = dabTheta / 2;
                float dabLength;
                // Should not happen; means that dab was 0 degrees
                if (sinf(dabPhi - baAngle) == 0)
                    dabLength = lineWidth;
                else
                    dabLength = lineWidth / sinf(dabPhi - baAngle);

                std::pair<float, float> trapPointD = {a.x + dabLength * cosf(dabPhi), a.y + dabLength * sinf(dabPhi)};
                C2D_DrawTriangle(a.x, a.y, foreground, b.x, b.y, foreground, trapPointD.first, trapPointD.second, foreground, CUR_DEPTH);

                // Angle of line segment AB with respect to BC
                float abcTheta = abAngle + bcAngle;
                float abcPhi   = abcTheta / 2;
                float abcLength;
                // Should not happen; means that abc was 0 degrees
                if (sinf(abcPhi - bcAngle) == 0)
                    abcLength = lineWidth;
                else
                    abcLength = lineWidth / sinf(abcPhi - bcAngle);

                std::pair<float, float> trapPointC = {b.x + abcLength * cosf(abcPhi), b.y + abcLength * sinf(abcPhi)};
                C2D_DrawTriangle(b.x, b.y, foreground, trapPointD.first, trapPointD.second, foreground, trapPointC.first, trapPointC.second, foreground, CUR_DEPTH);
            }
        }
    }
}

void Primitives::Scissor(bool enable, float x, float y, float width, float height)
{
    GPU_SCISSORMODE mode = (enable) ? GPU_SCISSOR_NORMAL : GPU_SCISSOR_DISABLE;

    auto windowModule = Module::GetInstance<Window>(Module::M_WINDOW);
    float screenWidth = (windowModule->GetDisplay() < 2) ? 400 : 320;

    C3D_SetScissor(mode, 240 - (y + height), screenWidth - (x + width), 240 - y, screenWidth - x);
}
