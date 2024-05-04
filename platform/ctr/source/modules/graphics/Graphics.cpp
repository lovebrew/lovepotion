#include "driver/display/Renderer.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/attributes.hpp"

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

    Graphics::~Graphics()
    {}

    void Graphics::clear(OptionalColor color, OptionalInt depth, OptionalDouble stencil)
    {
        Renderer::getInstance().bindFramebuffer();

        if (color.hasValue)
        {
            gammaCorrectColor(color.value);
            Renderer::getInstance().clear(color.value);
        }

        if (stencil.hasValue && depth.hasValue)
            Renderer::getInstance().clearDepthStencil(stencil.value, 0xFF, depth.value);
    }

    void Graphics::clear(const std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth)
    {
        if (colors.size() == 0 && !stencil.hasValue && !depth.hasValue)
            return;

        const int numColors = (int)colors.size();

        if (numColors <= 1)
        {
            this->clear(colors.size() > 0 ? colors[0] : OptionalColor(), stencil, depth);
            return;
        }

        this->flushBatchedDraws();
    }

    void Graphics::present()
    {
        if (!this->isActive())
            return;

        this->flushBatchedDraws();
        Renderer::getInstance().present();

        this->batchedDrawState.vertexBuffer->nextFrame();
        this->batchedDrawState.indexBuffer->nextFrame();

        this->drawCalls        = 0;
        this->drawCallsBatched = 0;
    }

    void Graphics::setScissor(const Rect& scissor)
    {
        GraphicsBase::setScissor(scissor);
        Renderer::getInstance().setScissor(scissor);
    }

    void Graphics::setScissor()
    {
        GraphicsBase::setScissor();
        Renderer::getInstance().setScissor(Rect::EMPTY);
    }

    void Graphics::setFrontFaceWinding(Winding winding)
    {
        GraphicsBase::setFrontFaceWinding(winding);
        Renderer::getInstance().setVertexWinding(winding);
    }

    void Graphics::setColorMask(ColorChannelMask mask)
    {
        GraphicsBase::setColorMask(mask);
        Renderer::getInstance().setColorMask(mask);
    }

    void Graphics::setBlendState(const BlendState& state)
    {
        GraphicsBase::setBlendState(state);

        if (state.enable)
            Renderer::getInstance().setBlendState(state);
    }

    bool Graphics::setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                           bool backBufferDepth, int msaa)
    {
        Renderer::getInstance().initialize();

        // clang-format off
        if (this->batchedDrawState.vertexBuffer == nullptr)
        {
            this->batchedDrawState.indexBuffer  = new StreamBuffer(BUFFERUSAGE_INDEX, INIT_INDEX_BUFFER_SIZE);
            this->batchedDrawState.vertexBuffer = new StreamBuffer(BUFFERUSAGE_VERTEX, INIT_VERTEX_BUFFER_SIZE);
        }
        // clang-format on

        this->created = true;

        if (!Volatile::loadAll())
            std::printf("Failed to load all volatile objects.\n");

        this->restoreState(this->states.back());

        for (int index = 0; index < 1; index++)
        {
            auto type = (Shader::StandardShader)index;

            try
            {
                Shader::standardShaders[type] = new Shader();
            }
            catch (const std::exception& e)
            {
                throw;
            }
        }

        if (!Shader::current)
            Shader::standardShaders[Shader::STANDARD_DEFAULT]->attach();

        return true;
    }

    void Graphics::unsetMode()
    {
        if (!this->isCreated())
            return;

        this->flushBatchedDraws();
    }

    bool Graphics::isActive() const
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);
        return this->active && this->created && window != nullptr && window->isOpen();
    }

    void Graphics::setViewport(int x, int y, int width, int height)
    {
        Renderer::getInstance().setViewport({ x, y, width, height }, true);
    }

    TextureBase* Graphics::newTexture(const TextureBase::Settings& settings, const TextureBase::Slices* data)
    {
        return new Texture(this, settings, data);
    }

    void Graphics::points(Vector2* positions, const Color* colors, int count)
    {
        const auto pointSize = this->states.back().pointSize;

        for (int index = 0; index < count; index++)
        {
            const auto& position = positions[index];
            auto& color          = colors[index];

            gammaCorrectColor(this->getColor());

            if (isGammaCorrect())
            {
                Color current = colors[index];

                gammaCorrectColor(current);
                current *= color;
                unGammaCorrectColor(current);

                this->setColor(current);
            }
            else
                this->setColor(color);

            this->circle(DRAW_FILL, position.x, position.y, pointSize);
        }
    }

    void Graphics::draw(const DrawIndexedCommand& command)
    {
        Renderer::getInstance().prepareDraw();

        if (command.texture != nullptr)
        {
            setTexEnvAttribute(TEXENV_MODE_TEXTURE);
            C3D_TexBind(0, (C3D_Tex*)command.texture->getHandle());
        }
        else
            setTexEnvAttribute(TEXENV_MODE_PRIMITIVE);

        const void* elements     = (const void*)command.indexBufferOffset;
        const auto primitiveType = Renderer::getPrimitiveType(command.primitiveType);
        const auto dataType      = Renderer::getIndexType(command.indexType);

        C3D_DrawElements(primitiveType, command.indexCount, dataType, elements);
        ++this->drawCalls;
    }

    bool Graphics::isPixelFormatSupported(PixelFormat format, uint32_t usage)
    {
        format        = this->getSizedFormat(format);
        bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;

        GPU_TEXCOLOR color;
        bool supported = Renderer::getConstant(format, color);

        return readable && supported;
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
