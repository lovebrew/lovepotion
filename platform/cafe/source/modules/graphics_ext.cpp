#include <modules/graphics_ext.hpp>
#include <utilities/driver/renderer_ext.hpp>

#include <modules/window_ext.hpp>

using Renderer = love::Renderer<love::Console::CAFE>;
using namespace love;

Graphics<Console::CAFE>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::CAFE>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
            window->SetWindow();
    }
}

void Graphics<Console::CAFE>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
{
    ::Renderer::Instance().BindFramebuffer();

    if (color.has_value())
    {
        /* todo: Graphics<>::GammaCorrectColor(color); */
        ::Renderer::Instance().Clear(color.value());
    }

    if (stencil.has_value() && depth.has_value())
        ::Renderer::Instance().ClearDepthStencil(stencil.value(), 0xFF, depth.value());
}

void Graphics<Console::CAFE>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                    OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::CAFE>::Present()
{
    ::Renderer::Instance().Present();
}

void Graphics<Console::CAFE>::SetMode(int x, int y, int width, int height)
{
    ::Renderer::Instance().CreateFramebuffers();
    this->RestoreState(this->states.back());
}
