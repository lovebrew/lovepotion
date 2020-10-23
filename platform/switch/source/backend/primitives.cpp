#include "common/runtime.h"
#include "common/backend/primitives.h"

using namespace love;

#define RAD_TO_DEG(r) ((r) * 180 / M_PI)

// #include <SDL2_gfxPrimitives.h>

#define WINDOW_MODULE() (Module::GetInstance<Window>(Module::M_WINDOW))

void Primitives::Rectangle(const std::string & mode, float x, float y, float width, float height, float rx, float ry, float lineWidth, const Colorf & color)
{}

void Primitives::Arc(const std::string & mode, float x, float y, float radius, float startAngle, float endAngle, const Colorf & color)
{}

void Primitives::Ellipse(const std::string & mode, float x, float y, float radiusX, float radiusY, const Colorf & color)
{}

void Primitives::Circle(const std::string & mode, float x, float y, float radius, float lineWidth, const Colorf & color)
{}

void Primitives::Polygon(const std::string & mode, std::vector<Graphics::Point> points, float lineWidth, const Colorf & color)
{}

void Primitives::Line(float x1, float y1, float x2, float y2, float lineWidth, const Colorf & color)
{}

void Primitives::Scissor(bool enabled, int x, int y, int width, int height)
{}
