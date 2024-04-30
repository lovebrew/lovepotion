#include "driver/display/Renderer.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/window/Window.hpp"

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

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

        this->flushBatchedDraws();
    }

    void Graphics::present()
    {
        if (!this->isActive())
            return;

        this->flushBatchedDraws();
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

        this->created = true;

        if (!Volatile::loadAll())
            std::printf("Failed to load all volatile objects.\n");

        // clang-format off
        if (this->batchedDrawState.vb[0] == nullptr)
        {
            this->batchedDrawState.vb[0] = createStreamBuffer(BUFFERUSAGE_VERTEX, 1024 * 1024 * 1);
            this->batchedDrawState.vb[1] = createStreamBuffer(BUFFERUSAGE_VERTEX, 256 * 1024 * 1);
            this->batchedDrawState.indexBuffer = createStreamBuffer(BUFFERUSAGE_INDEX, sizeof(uint16_t) * LOVE_UINT16_MAX);
        }
        // clang-format on

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

    void Graphics::draw(Drawable* drawable, const Matrix4& matrix)
    {
        drawable->draw(*this, matrix);
    }

    void Graphics::draw(TextureBase* texture, Quad* quad, const Matrix4& matrix)
    {
        texture->draw(*this, quad, matrix);
    }

    bool Graphics::isPixelFormatSupported(PixelFormat format, uint32_t usage)
    {
        format        = this->getSizedFormat(format);
        bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;

        GPU_TEXCOLOR color;
        bool supported = Renderer::getConstant(format, color);

        return readable && supported;
    }

    void Graphics::draw(const DrawCommand& command)
    {
        // C3D_SetBufInfo((C3D_BufInfo*)command->getHandle());

        // auto* env = C3D_GetTexEnv(0);
        // C3D_TexEnvInit(env);

        // if (command.texture == nullptr)
        // {
        //     C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        //     C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
        // }
        // else
        // {
        //     C3D_TexBind(0, (C3D_Tex*)command.texture->getHandle());
        //     C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
        //     C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
        // }

        // GPU_Primitive_t primitive;
        // if (!Renderer::getConstant(command.primitiveType, primitive))
        //     throw love::Exception("Invalid primitive type: {:d}", (int)command.primitiveType);

        // C3D_DrawArrays(primitive, command.vertexStart, command.vertexCount);

        // ++this->drawCalls;
    }

    void Graphics::draw(const DrawIndexedCommand& command)
    {
        std::printf("Binding C3D_BufInfo\n");
        C3D_SetBufInfo((C3D_BufInfo*)command.indexBuffer->getHandle());
        std::printf("Resetting TexEnv\n");
        auto* env = C3D_GetTexEnv(0);
        C3D_TexEnvInit(env);

        if (command.texture == nullptr)
        {
            std::printf("Setting up texture environment for no texture\n");
            C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
            C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
        }
        else
        {
            std::printf("Setting up texture environment for texture\n");
            C3D_TexBind(0, (C3D_Tex*)command.texture->getHandle());
            C3D_TexEnvSrc(env, C3D_Both, GPU_TEXTURE0, GPU_PRIMARY_COLOR, GPU_PRIMARY_COLOR);
            C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
        }

        GPU_Primitive_t primitive;
        if (!Renderer::getConstant(command.primitiveType, primitive))
            throw love::Exception("Invalid primitive type: {:d}", (int)command.primitiveType);

        decltype(C3D_UNSIGNED_BYTE) type;
        if (!Renderer::getConstant(command.indexType, type))
            throw love::Exception("Invalid index type: {:d}", (int)command.indexType);
        std::printf("Drawing elements\n");
        const void* indices = BUFFER_OFFSET(command.indexBufferOffset);
        C3D_DrawElements(primitive, command.indexCount, type, indices);

        ++this->drawCalls;
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
