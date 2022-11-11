#include <modules/graphics_ext.hpp>
#include <utilities/driver/renderer_ext.hpp>

#include <modules/window_ext.hpp>

using Renderer = love::Renderer<love::Console::HAC>;
using namespace love;

Graphics<Console::HAC>::Graphics()
{
    auto* window = Module::GetInstance<Window<Console::HAC>>(Module::M_WINDOW);

    if (window != nullptr)
    {
        window->SetGraphics(this);

        if (window->IsOpen())
        {
            int width, height;
            Window<>::WindowSettings settings {};

            window->GetWindow(width, height, settings);
            window->SetWindow(width, height, &settings);
        }
    }
}

void Graphics<Console::HAC>::Clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
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

void Graphics<Console::HAC>::Clear(std::vector<OptionalColor>& colors, OptionalInt stencil,
                                   OptionalDouble depth)
{
    int colorCount = colors.size();

    if (colorCount == 0 || !stencil.has_value() || !depth.has_value())
        this->Clear(colorCount > 0 ? colors[0] : Color {}, stencil, depth);
}

void Graphics<Console::HAC>::Present()
{
    ::Renderer::Instance().Present();
}

void Graphics<Console::HAC>::SetMode(int x, int y, int width, int height)
{
    this->width  = width;
    this->height = height;

    ::Renderer::Instance().CreateFramebuffers();
    this->SetViewportSize(width, height);

    this->RestoreState(this->states.back());
}

void Graphics<Console::HAC>::SetViewportSize(int width, int height)
{
    ::Renderer::Instance().SetViewport({ 0, 0, width, height });
    ::Renderer::Instance().SetScissor(true, { 0, 0, width, height }, false);
}

void Graphics<Console::HAC>::SetScissor(const Rect& scissor)
{
    Graphics<Console::ALL>::SetScissor(scissor);
    ::Renderer::Instance().SetScissor(this->states.back().scissor.active, scissor, false);
}
