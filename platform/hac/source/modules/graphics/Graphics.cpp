#include "driver/display/deko.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/Shader.hpp"

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

    void Graphics::backbufferChanged(int width, int height, int pixelWidth, int pixelHeight, bool stencil,
                                     bool depth, int msaa)
    {
        bool changed = width != this->width || height != this->height || pixelWidth != this->pixelWidth ||
                       pixelHeight != this->pixelHeight;

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
            d3d.setViewport({ 0, 0, pixelWidth, pixelHeight });

            if (this->states.back().scissor)
                this->setScissor(this->states.back().scissorRect);

            this->resetProjection();
        }

        if (!changed)
            return;

        d3d.onModeChanged();
    }

    void Graphics::clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
    {
        if (color.hasValue)
        {
            bool hasIntegerFormat = false;
            const auto& targets   = this->states.back().renderTargets;

            for (const auto& target : targets.colors)
            {
                if (target.texture.get() && love::isPixelFormatInteger(target.texture->getPixelFormat()))
                    hasIntegerFormat = true;

                if (hasIntegerFormat)
                {
                    std::vector<OptionalColor> colors(targets.colors.size());

                    for (size_t index = 0; index < colors.size(); index++)
                        colors[index] = color;

                    this->clear(colors, stencil, depth);
                    return;
                }
            }
        }

        if (color.hasValue || stencil.hasValue || depth.hasValue)
            this->flushBatchedDraws();

        if (color.hasValue)
        {
            gammaCorrectColor(color.value);
            d3d.clear(color.value);
        }

        d3d.bindFramebuffer(d3d.getInternalBackbuffer());
    }

    void Graphics::clear(const std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth)
    {
        if (colors.size() == 0 && !stencil.hasValue && !depth.hasValue)
            return;

        int numColors = (int)colors.size();

        const auto& targets       = this->states.back().renderTargets.colors;
        const int numColorTargets = targets.size();

        if (numColors <= 1 &&
            (numColorTargets == 0 || (numColorTargets == 1 && targets[0].texture.get() != nullptr &&
                                      !isPixelFormatInteger(targets[0].texture->getPixelFormat()))))
        {
            this->clear(colors.size() > 0 ? colors[0] : OptionalColor(), stencil, depth);
            return;
        }

        this->flushBatchedDraws();

        numColors = std::min(numColors, numColorTargets);

        for (int index = 0; index < numColors; index++)
        {
            OptionalColor current = colors[index];

            if (!current.hasValue)
                continue;

            Color value(current.value.r, current.value.g, current.value.b, current.value.a);

            gammaCorrectColor(value);
            d3d.clear(value);
        }

        d3d.bindFramebuffer(d3d.getInternalBackbuffer());
    }

    void Graphics::present(void* screenshotCallback)
    {
        if (!this->isActive())
            return;

        if (this->isRenderTargetActive())
            throw love::Exception("present cannot be called while a render target is active.");

        d3d.present();

        this->drawCalls        = 0;
        this->drawCallsBatched = 0;
    }

    void Graphics::setScissor(const Rect& scissor)
    {
        this->flushBatchedDraws();

        auto& state     = this->states.back();
        double dpiscale = this->getCurrentDPIScale();

        Rect rectangle {};
        rectangle.x = scissor.x * dpiscale;
        rectangle.y = scissor.y * dpiscale;
        rectangle.w = scissor.w * dpiscale;
        rectangle.h = scissor.h * dpiscale;

        d3d.setScissor(rectangle);

        state.scissor     = true;
        state.scissorRect = scissor;
    }

    void Graphics::setScissor()
    {
        if (this->states.back().scissor)
            this->flushBatchedDraws();

        this->states.back().scissor = false;
        d3d.setScissor(Rect::EMPTY);
    }

    void Graphics::setFrontFaceWinding(Winding winding)
    {
        auto& state = this->states.back();

        if (state.winding != winding)
            this->flushBatchedDraws();

        state.winding = winding;

        if (this->isRenderTargetActive())
            winding = (winding == WINDING_CW) ? WINDING_CCW : WINDING_CW; // ???

        d3d.setVertexWinding(winding);
    }

    void Graphics::setColorMask(ColorChannelMask mask)
    {
        this->flushBatchedDraws();

        d3d.setColorMask(mask);
        this->states.back().colorMask = mask;
    }

    void Graphics::setBlendState(const BlendState& state)
    {
        if (!(state == this->states.back().blend))
            this->flushBatchedDraws();

        if (state.operationRGB == BLENDOP_MAX || state.operationA == BLENDOP_MAX ||
            state.operationRGB == BLENDOP_MIN || state.operationA == BLENDOP_MIN)
        {
            if (!capabilities.features[FEATURE_BLEND_MINMAX])
                throw love::Exception(E_BLEND_MIN_MAX_NOT_SUPPORTED);
        }

        if (state.enable)
            d3d.setBlendState(state);

        this->states.back().blend = state;
    }

    bool Graphics::setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                           bool backBufferDepth, int msaa)
    {
        d3d.initialize();

        this->created = true;
        this->initCapabilities();

        // d3d.setupContext();

        try
        {
            if (this->batchedDrawState.vertexBuffer == nullptr)
            {
                this->batchedDrawState.indexBuffer  = newIndexBuffer(INIT_INDEX_BUFFER_SIZE);
                this->batchedDrawState.vertexBuffer = newVertexBuffer(INIT_VERTEX_BUFFER_SIZE);
            }
        }
        catch (love::Exception&)
        {
            throw;
        }

        if (!Volatile::loadAll())
            std::printf("Failed to load all volatile objects.\n");

        this->restoreState(this->states.back());

        for (int index = 0; index < Shader::STANDARD_MAX_ENUM; index++)
        {
            auto type = (Shader::StandardShader)index;

            try
            {
                Shader::standardShaders[type] = new Shader(type);
            }
            catch (const std::exception& e)
            {
                throw;
            }
        }

        if (!Shader::current)
            Shader::standardShaders[Shader::STANDARD_DEFAULT]->attach();

        return true;

        this->backbufferChanged(width, height, pixelWidth, pixelHeight, backBufferStencil, backBufferDepth,
                                msaa);

        return true;
    }

    void Graphics::unsetMode()
    {}

    bool Graphics::isActive() const
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);
        return this->active && this->created && window != nullptr && window->isOpen();
    }
} // namespace love
