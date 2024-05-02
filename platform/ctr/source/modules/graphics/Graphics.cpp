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

    void Graphics::clear(OptionalColor color, OptionalInt depth, OptionalDouble stencil)
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
    }

    void Graphics::present()
    {
        if (!this->isActive())
            return;

        Renderer::getInstance().present();

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
        Renderer::getInstance().setBlendState(state);
    }

    bool Graphics::setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                           bool backBufferDepth, int msaa)
    {
        Renderer::getInstance().initialize();
        Renderer::getInstance().setupContext(this->batchedDrawState);

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

    Texture* Graphics::newTexture(const Texture::Settings& settings, const Texture::Slices* data)
    {
        return new Texture(*this, settings, data);
    }

    void Graphics::points(Vector2* positions, const Color* colors, int count)
    {
        // Vector2* coords = positions;

        // float twoPi = float(LOVE_M_PI * 2);
        // float shift = twoPi / count;
        // float phi   = 0.0f;

        // float x = coords[0].x;
        // float y = coords[0].y;

        // float radius = this->states.back().pointSize;

        // for (int index = 0; index < count; ++index, phi += shift)
        // {
        //     coords[index].x = x + radius * std::cos(phi);
        //     coords[index].y = y + radius * std::sin(phi);
        // }

        // coords[count] = coords[0];
        // this->polygon(DRAW_FILL, std::span(coords, count + 2), false);
    }

    void Graphics::draw(Drawable* drawable, const Matrix4& matrix)
    {
        drawable->draw(*this, matrix);
    }

    void Graphics::draw(Texture* texture, Quad* quad, const Matrix4& matrix)
    {
        texture->draw(*this, quad, matrix);
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

        GPU_Primitive_t primitiveType;
        if (!Renderer::getConstant(command.primitiveType, primitiveType))
            throw love::Exception("Invalid primitive type: {:d}.", (int)command.primitiveType);

        decltype(C3D_UNSIGNED_BYTE) indexType;
        if (!Renderer::getConstant(command.indexType, indexType))
            throw love::Exception("Invalid index type: {:d}.", (int)command.indexType);

        const void* elements = (const void*)command.indexBufferOffset;
        C3D_DrawElements(primitiveType, command.indexCount, indexType, elements);
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
