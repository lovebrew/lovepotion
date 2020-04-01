#include "common/runtime.h"
#include "common/backend/primitives.h"

using namespace love;

#include <SDL2_gfxPrimitives.h>

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

void Primitives::Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry, float lineWidth, const Color & color)
{
    if (mode == "fill")
        boxRGBA(WINDOW_MODULE()->GetRenderer(), roundf(x), roundf(y), roundf(x + width - 1), roundf(y + height - 1), color.r, color.g, color.b, color.a);
    else
        rectangleRGBA(WINDOW_MODULE()->GetRenderer(), roundf(x), roundf(y), x + width - 1, y + height - 1, color.r, color.g, color.b, color.a);
}

void Primitives::Circle(const std::string & mode, float x, float y, float radius, float lineWidth, const Color & color)
{
    if (mode == "fill")
        filledCircleRGBA(WINDOW_MODULE()->GetRenderer(), x, y, radius, color.r, color.g, color.b, color.a);
    else if (mode == "line")
        circleRGBA(WINDOW_MODULE()->GetRenderer(), x, y, radius, color.r, color.g, color.b, color.a);
}

void Primitives::Polygon(const std::string & mode, std::vector<Graphics::Point> points, float lineWidth, const Color & color)
{
    s16 xCoords[points.size() + 1];
    s16 yCoords[points.size() + 1];

    for (size_t index = 0; index < points.size(); index++)
    {
        xCoords[index] = points[index].x;
        yCoords[index] = points[index].y;
    }

    xCoords[points.size()] = points[0].x;
    yCoords[points.size()] = points[0].y;

    if (mode == "fill")
        filledPolygonRGBA(WINDOW_MODULE()->GetRenderer(), xCoords, yCoords, points.size(), color.r, color.g, color.b, color.a);
    else if (mode == "line")
        polygonRGBA(WINDOW_MODULE()->GetRenderer(), xCoords, yCoords, points.size(), color.r, color.g, color.b, color.a);
}

void Primitives::Line(float x1, float y1, float x2, float y2, float lineWidth, const Color & color)
{
    thickLineRGBA(WINDOW_MODULE()->GetRenderer(), x1, y1, x2, y2, lineWidth, color.r, color.g, color.b, color.a);
}

void Primitives::Scissor(bool enabled, float x, float y, float width, float height)
{
    SDL_Rect clip({x, y, width, height});

    if (!enabled)
        SDL_RenderSetClipRect(WINDOW_MODULE()->GetRenderer(), NULL);
    else
        SDL_RenderSetClipRect(WINDOW_MODULE()->GetRenderer(), &clip);
}
