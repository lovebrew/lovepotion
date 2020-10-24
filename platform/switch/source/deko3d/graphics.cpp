#include "common/runtime.h"
#include "deko3d/graphics.h"

using namespace love;

void love::deko3d::Graphics::Clear(std::optional<Colorf> color, std::optional<int> stencil, std::optional<double> depth)
{
    if (color.has_value() || stencil.has_value() || depth.has_value())
    {} // flush stream draws

    if (color.has_value())
        dk3d.Clear(color, stencil, depth);
}

void love::deko3d::Graphics::Present()
{
    dk3d.Present();
}