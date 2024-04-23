#include "driver/display/Renderer.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.hpp"

namespace love
{
    Graphics::Graphics() :
        GraphicsBase("love.graphics.deko3d"),
        backBufferHasStencil(false),
        backBufferHasDepth(false),
        requestedBackbufferMSAA(0)
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

    void Graphics::backbufferChanged(int width, int height, int pixelWidth, int pixelHeight,
                                     bool stencil, bool depth, int msaa)
    {
        bool changed = width != this->width || height != this->height ||
                       pixelWidth != this->pixelWidth || pixelHeight != this->pixelHeight;

        changed |= stencil != this->backBufferHasStencil || depth != this->backBufferHasDepth;
        changed |= msaa != this->requestedBackbufferMSAA;

        this->width  = width;
        this->height = height;

        this->pixelWidth  = pixelWidth;
        this->pixelHeight = pixelHeight;

        this->backBufferHasStencil    = stencil;
        this->backBufferHasDepth      = depth;
        this->requestedBackbufferMSAA = msaa;

        if (!this->isRenderTargetActive())
        {
            Renderer::getInstance().setViewport({ 0, 0, pixelWidth, pixelHeight });

            if (this->states.back().scissor)
                this->setScissor(this->states.back().scissorRect);

            this->resetProjection();
        }

        if (!changed)
            return;

        Renderer::getInstance().onModeChanged();
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

        this->backbufferChanged(width, height, pixelWidth, pixelHeight, backBufferStencil,
                                backBufferDepth, msaa);

        return true;
    }

    void Graphics::unsetModeImpl()
    {}

    bool Graphics::isActive() const
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);
        return this->active && this->created && window != nullptr && window->isOpen();
    }
} // namespace love
