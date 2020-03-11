/*
** render.h
** Renderer superclass
*/

#pragma once

#include "modules/window/window.h"
#include "modules/graphics/graphics.h"

namespace Primitives
{
    void Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry, float lineWidth, const Color & color);

    void Circle(const std::string & mode, float x, float y, float radius, float lineWidth, const Color & color);

    void Polygon(const std::string & mode, std::vector<love::Graphics::Point> points, float lineWidth, const Color & color);

    void Line(float x1, float y1, float x2, float y2, float lineWidth, const Color & color);

    void Points(std::vector<std::vector<float>> points, const Color & color);

    void Points(std::vector<float> points, const Color & color);

    void Scissor(bool enabled, float x, float y, float width, float height);

    void Stencil();
}
