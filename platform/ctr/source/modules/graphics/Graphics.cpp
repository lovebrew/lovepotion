#include "driver/display/citro3d.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/Font.hpp"

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

    void Graphics::initCapabilities()
    {
        // clang-format off
        this->capabilities.features[FEATURE_MULTI_RENDER_TARGET_FORMATS]  = false;
        this->capabilities.features[FEATURE_CLAMP_ZERO]                   = true;
        this->capabilities.features[FEATURE_CLAMP_ONE]                    = true;
        this->capabilities.features[FEATURE_BLEND_MINMAX]                 = true;
        this->capabilities.features[FEATURE_LIGHTEN]                      = true;
        this->capabilities.features[FEATURE_FULL_NPOT]                    = true;
        this->capabilities.features[FEATURE_PIXEL_SHADER_HIGHP]           = false;
        this->capabilities.features[FEATURE_SHADER_DERIVATIVES]           = false;
        this->capabilities.features[FEATURE_GLSL3]                        = false;
        this->capabilities.features[FEATURE_GLSL4]                        = false;
        this->capabilities.features[FEATURE_INSTANCING]                   = false;
        this->capabilities.features[FEATURE_TEXEL_BUFFER]                 = false;
        this->capabilities.features[FEATURE_INDEX_BUFFER_32BIT]           = false;
        this->capabilities.features[FEATURE_COPY_BUFFER_TO_TEXTURE]       = false; //< might be possible
        this->capabilities.features[FEATURE_COPY_TEXTURE_TO_BUFFER]       = false; //< might be possible
        this->capabilities.features[FEATURE_COPY_RENDER_TARGET_TO_BUFFER] = false; //< might be possible
        this->capabilities.features[FEATURE_MIPMAP_RANGE]                 = false;
        this->capabilities.features[FEATURE_INDIRECT_DRAW]                = false;
        static_assert(FEATURE_MAX_ENUM == 19,  "Graphics::initCapabilities must be updated when adding a new graphics feature!");

        this->capabilities.limits[LIMIT_POINT_SIZE]                 = 8.0f;
        this->capabilities.limits[LIMIT_TEXTURE_SIZE]               = LOVE_TEX3DS_MAX;
        this->capabilities.limits[LIMIT_TEXTURE_LAYERS]             = 1;
        this->capabilities.limits[LIMIT_VOLUME_TEXTURE_SIZE]        = LOVE_TEX3DS_MAX;
        this->capabilities.limits[LIMIT_CUBE_TEXTURE_SIZE]          = LOVE_TEX3DS_MAX;
        this->capabilities.limits[LIMIT_TEXEL_BUFFER_SIZE]          = 0;
        this->capabilities.limits[LIMIT_SHADER_STORAGE_BUFFER_SIZE] = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_X]             = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_Y]             = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_Z]             = 0;
        this->capabilities.limits[LIMIT_RENDER_TARGETS]             = 1; //< max simultaneous render targets
        this->capabilities.limits[LIMIT_TEXTURE_MSAA]               = 0;
        this->capabilities.limits[LIMIT_ANISOTROPY]                 = 0;
        static_assert(LIMIT_MAX_ENUM == 13, "Graphics::initCapabilities must be updated when adding a new system limit!");
        // clang-format on

        this->capabilities.textureTypes[TEXTURE_2D]       = true;
        this->capabilities.textureTypes[TEXTURE_VOLUME]   = false;
        this->capabilities.textureTypes[TEXTURE_CUBE]     = true;
        this->capabilities.textureTypes[TEXTURE_2D_ARRAY] = false;
    }

    void Graphics::setActiveScreen()
    {
        c3d.ensureInFrame();
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
            c3d.clear(color.value);
        }

        c3d.bindFramebuffer(c3d.getInternalBackbuffer());
    }

    C3D_RenderTarget* Graphics::getInternalBackbuffer() const
    {
        return c3d.getInternalBackbuffer();
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
            c3d.clear(value);
        }

        c3d.bindFramebuffer(c3d.getInternalBackbuffer());
    }

    void Graphics::present(void* screenshotCallbackData)
    {
        if (!this->isActive())
            return;

        if (this->isRenderTargetActive())
            throw love::Exception("present cannot be called while a render target is active.");

        c3d.present();

        this->drawCalls        = 0;
        this->drawCallsBatched = 0;

        this->cpuProcessingTime = C3D_GetProcessingTime();
        this->gpuDrawingTime    = C3D_GetDrawingTime();
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

        c3d.setScissor(rectangle);

        state.scissor     = true;
        state.scissorRect = scissor;
    }

    void Graphics::setScissor()
    {
        if (this->states.back().scissor)
            this->flushBatchedDraws();

        this->states.back().scissor = false;
        c3d.setScissor(Rect::EMPTY);
    }

    void Graphics::setFrontFaceWinding(Winding winding)
    {
        auto& state = this->states.back();

        if (state.winding != winding)
            this->flushBatchedDraws();

        state.winding = winding;

        if (this->isRenderTargetActive())
            winding = (winding == WINDING_CW) ? WINDING_CCW : WINDING_CW; // ???

        c3d.setVertexWinding(winding);
    }

    void Graphics::setColorMask(ColorChannelMask mask)
    {
        this->flushBatchedDraws();

        c3d.setColorMask(mask);
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
            c3d.setBlendState(state);

        this->states.back().blend = state;
    }

    FontBase* Graphics::newFont(Rasterizer* data)
    {
        return new Font(data, this->states.back().defaultSamplerState);
    }

    FontBase* Graphics::newDefaultFont(int size, const Rasterizer::Settings& settings)
    {
        auto* module = Module::getInstance<FontModuleBase>(Module::M_FONT);

        if (module == nullptr)
            throw love::Exception("Font module has not been loaded.");

        StrongRef<Rasterizer> rasterizer = module->newTrueTypeRasterizer(size, settings);

        return this->newFont(rasterizer.get());
    }

    ShaderStageBase* Graphics::newShaderStageInternal(ShaderStageType stage, const std::string& filepath)
    {
        return new ShaderStage(stage, filepath);
    }

    ShaderBase* Graphics::newShaderInternal(StrongRef<ShaderStageBase> stages[SHADERSTAGE_MAX_ENUM],
                                            const ShaderBase::CompileOptions& options)
    {
        return new Shader(stages, options);
    }

    bool Graphics::setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                           bool backBufferDepth, int msaa)
    {
        c3d.initialize();

        this->created = true;
        this->initCapabilities();

        c3d.setupContext();

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

        for (int index = 0; index < 1; index++)
        {
            const auto type = (Shader::StandardShader)index;

            if (!Shader::standardShaders[index])
            {
                std::vector<std::string> stages {};
                Shader::CompileOptions options {};

                stages.push_back(Shader::getDefaultStagePath(type, SHADERSTAGE_VERTEX));

                try
                {
                    Shader::standardShaders[index] = this->newShader(stages, options);
                }
                catch (const std::exception& e)
                {
                    throw;
                }
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
        c3d.deInitialize();
    }

    void Graphics::setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                            bool hasSRGBTexture)
    {
        const auto& state = this->states.back();

        bool isWindow = targets.getFirstTarget().texture == nullptr;

        if (isWindow)
            c3d.bindFramebuffer(c3d.getInternalBackbuffer());
        else
            c3d.bindFramebuffer((C3D_RenderTarget*)targets.getFirstTarget().texture->getRenderTargetHandle());

        bool tilt = isWindow ? true : false;
        c3d.setViewport(pixelWidth, pixelHeight, tilt);

        if (state.scissor)
            c3d.setScissor(state.scissorRect);
    }

    bool Graphics::isActive() const
    {
        auto* window = Module::getInstance<Window>(M_WINDOW);
        return this->active && this->created && window != nullptr && window->isOpen();
    }

    void Graphics::setViewport(int x, int y, int width, int height)
    {
        c3d.setViewport(width, height, true);
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

            if (!colors)
            {
                this->circle(DRAW_FILL, position.x, position.y, pointSize);
                return;
            }

            auto& color = colors[index];

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

    bool Graphics::isPixelFormatSupported(PixelFormat format, uint32_t usage)
    {
        format        = this->getSizedFormat(format);
        bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;

        GPU_TEXCOLOR color;
        bool supported = citro3d::getConstant(format, color);

        return readable && supported;
    }

    void Graphics::draw(const DrawIndexedCommand& command)
    {
        c3d.prepareDraw(this);
        // c3d.setVertexAttributes(*command.attributes, *command.buffers);
        c3d.bindTextureToUnit(command.texture, 0, command.isFont);

        const auto* indices = (const uint16_t*)command.indexBuffer->getHandle();
        const size_t offset = command.indexBufferOffset;

        const auto primitiveType = citro3d::getPrimitiveType(command.primitiveType);
        const auto dataType      = C3D_UNSIGNED_SHORT;

        C3D_DrawElements(primitiveType, command.indexCount, dataType, &indices[offset]);

        ++this->drawCalls;
    }

    void Graphics::draw(const DrawCommand& command)
    {
        c3d.prepareDraw(this);
        // c3d.setVertexAttributes(*command.attributes, *command.buffers);
        c3d.bindTextureToUnit(command.texture, 0, command.isFont);

        const auto primitiveType = citro3d::getPrimitiveType(command.primitiveType);

        C3D_DrawArrays(primitiveType, command.vertexStart, command.vertexCount);
        ++this->drawCalls;
    }

    void Graphics::drawQuads(int start, int count, TextureBase* texture)
    {
        c3d.prepareDraw(this);
        c3d.bindTextureToUnit(texture, 0, false);
        c3d.setCullMode(CULL_NONE);

        for (int quadIndex = 0; quadIndex < count; quadIndex += MAX_QUADS_PER_DRAW)
        {
            int quadCount = std::min(MAX_QUADS_PER_DRAW, count - quadIndex);
            C3D_DrawElements(GPU_TRIANGLES, quadCount * 6, C3D_UNSIGNED_SHORT, nullptr);
        }
    }

    bool Graphics::is3D() const
    {
        return gfxIs3D();
    }

    void Graphics::set3D(bool enable)
    {
        c3d.set3DMode(enable);
    }

    bool Graphics::isWide() const
    {
        return gfxIsWide();
    }

    void Graphics::setWide(bool enable)
    {
        c3d.setWideMode(enable);
    }

    float Graphics::getDepth() const
    {
        return osGet3DSliderState();
    }
} // namespace love
