#include "driver/display/Renderer.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.hpp"

namespace love
{
    Graphics::Graphics() : GraphicsBase("love.graphics.citro3d")
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);

        if (window != nullptr)
        {
            window->setGraphics(this);

            if (window->isOpen())
            {
                int width, height;
                Window::WindowSettings settings {};

                window->getWindow(width, height, settings);
                window->setWindow(width, height, &settings);
            }
        }
    }

    void Graphics::clearImpl(OptionalColor color, OptionalInt depth, OptionalDouble stencil)
    {
        Renderer::getInstance().bindFramebuffer();

        if (color.hasValue)
        {
            // gammaCorrectColor(color.value);
            Renderer::getInstance().clear(color.value);
        }

        if (stencil.hasValue && depth.hasValue)
            Renderer::getInstance().clearDepthStencil(stencil.value, 0xFF, depth.value);
    }

    void Graphics::presentImpl()
    {
        Renderer::getInstance().present();
    }

    void Graphics::setScissorImpl(const Rect& scissor)
    {
        Renderer::getInstance().setScissor(scissor);
    }

    void Graphics::setScissorImpl()
    {
        Renderer::getInstance().setScissor(Rect::EMPTY);
    }

    void Graphics::setFrontFaceWindingImpl(Winding winding)
    {
        Renderer::getInstance().setVertexWinding(winding);
    }

    void Graphics::setColorMaskImpl(ColorChannelMask mask)
    {
        Renderer::getInstance().setColorMask(mask);
    }

    void Graphics::setBlendStateImpl(const BlendState& state)
    {
        Renderer::getInstance().setBlendState(state);
    }

    bool Graphics::setModeImpl(int width, int height, int pixelWidth, int pixelHeight,
                               bool backBufferStencil, bool backBufferDepth, int msaa)
    {
        Renderer::getInstance().initialize();

        return true;
    }

    void Graphics::unsetModeImpl()
    {}

    bool Graphics::isActive() const
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);
        return this->active && this->created && window != nullptr && window->isOpen();
    }

    void Graphics::setViewport(int x, int y, int width, int height)
    {
        Renderer::getInstance().setViewport({ x, y, width, height }, true);
    }

    bool Graphics::is3D() const
    {
        return gfxIs3D();
    }

    void Graphics::set3D(bool enable)
    {
        Renderer::getInstance().set3DMode(enable);
    }

    bool Graphics::isWide() const
    {
        return gfxIsWide();
    }

    void Graphics::setWide(bool enable)
    {
        Renderer::getInstance().setWideMode(enable);
    }

    float Graphics::getDepth() const
    {
        return osGet3DSliderState();
    }
} // namespace love
