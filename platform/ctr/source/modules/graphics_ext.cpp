#include <modules/graphics_ext.hpp>
#include <utilities/driver/renderer_ext.hpp>

using namespace love;

void Graphics<Console::CTR>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
{
    Renderer<Console::CTR>::Instance().BindFramebuffer();

    if (color.has_value())
    {
        /* todo: Graphics<>::GammaCorrectColor(color); */
        Renderer<Console::CTR>::Instance().Clear(color.value());
    }

    if (stencil.has_value() && depth.has_value())
        Renderer<Console::CTR>::Instance().ClearDepthStencil(stencil.value(), 0xFF, depth.value());
}

void Graphics<Console::CTR>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                   OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::CTR>::Present()
{
    Renderer<Console::CTR>::Instance().Present();
}
