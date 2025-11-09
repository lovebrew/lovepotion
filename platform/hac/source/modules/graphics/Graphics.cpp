#include "driver/display/deko.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"
#include "modules/graphics/Texture.hpp"
#include "modules/graphics/freetype/Font.hpp"

namespace love
{
    Graphics::Graphics() :
        GraphicsBase("love.graphics.deko3d"),
        backBufferHasStencil(false),
        backBufferHasDepth(false),
        requestedBackbufferMSAA(0)
    {
        auto* window = Module::getInstance<WindowBase>(M_WINDOW);

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
    {
        for (int index = 0; index < ShaderBase::STANDARD_MAX_ENUM; index++)
        {
            if (ShaderBase::standardShaders[index])
            {
                ShaderBase::standardShaders[index]->release();
                ShaderBase::standardShaders[index] = nullptr;
            }
        }

        this->states.clear();
        this->defaultFont.set(nullptr);

        if (this->batchedDrawState.vertexBuffer)
            this->batchedDrawState.vertexBuffer->release();

        if (this->batchedDrawState.indexBuffer)
            this->batchedDrawState.indexBuffer->release();

        d3d.deInitialize();
    }

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
        this->capabilities.limits[LIMIT_TEXTURE_SIZE]               = 4096;
        this->capabilities.limits[LIMIT_TEXTURE_LAYERS]             = 1;
        this->capabilities.limits[LIMIT_VOLUME_TEXTURE_SIZE]        = 4096;
        this->capabilities.limits[LIMIT_CUBE_TEXTURE_SIZE]          = 4096;
        this->capabilities.limits[LIMIT_TEXEL_BUFFER_SIZE]          = 0;
        this->capabilities.limits[LIMIT_SHADER_STORAGE_BUFFER_SIZE] = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_X]             = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_Y]             = 0;
        this->capabilities.limits[LIMIT_THREADGROUPS_Z]             = 0;
        this->capabilities.limits[LIMIT_RENDER_TARGETS]             = DK_MAX_RENDER_TARGETS; //< max simultaneous render targets
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
        d3d.ensureInFrame();
    }

    void Graphics::clear(OptionalColor color, OptionalInt stencil, OptionalDouble depth)
    {
        if (!color.hasValue && !stencil.hasValue && !depth.hasValue)
            return;

        std::vector<OptionalColor> colors {};

        if (color.hasValue)
            colors.resize(std::max(1, (int)this->states.back().renderTargets.colors.size()), color);

        this->clear(colors, stencil, depth);
    }

    void Graphics::clear(const std::vector<OptionalColor>& colors, OptionalInt stencil, OptionalDouble depth)
    {
        if (colors.size() == 0 && !stencil.hasValue && !depth.hasValue)
            return;

        this->flushBatchedDraws();

        const auto& targets  = this->states.back().renderTargets.colors;
        bool targetActive    = this->isRenderTargetActive();
        size_t nColorBuffers = targetActive ? targets.size() : 1;
        size_t nColors       = std::min(nColorBuffers, colors.size());

        d3d.bindFramebuffer();

        d3d.clear(colors[0].value);

        d3d.clearDepthStencil(stencil.value, depth.value);
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
        Shader::shaderSwitches = 0;
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

    void Graphics::setStencilState(const StencilState& state)
    {
        Graphics::flushBatchedDraws();
        d3d.setStencilState(state);

        this->states.back().stencil = state;
    }

    void Graphics::setDepthMode(CompareMode compare, bool write)
    {
        auto& state = this->states.back();
        if (state.depthTest != compare || state.depthWrite != write)
            Graphics::flushBatchedDraws();

        state.depthTest  = compare;
        state.depthWrite = write;

        d3d.setDepthWrites(compare, write);
    }

    void Graphics::setPointSize(float size)
    {
        if (size != this->states.back().pointSize)
            this->flushBatchedDraws();

        this->states.back().pointSize = size;
        d3d.setPointSize(size);
    }

    void Graphics::setFrontFaceWinding(Winding winding)
    {
        auto& state = this->states.back();

        if (state.winding != winding)
            this->flushBatchedDraws();

        state.winding = winding;

        if (this->isRenderTargetActive())
            winding = (winding == WINDING_CW) ? WINDING_CCW : WINDING_CW;

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

    bool Graphics::isPixelFormatSupported(PixelFormat format, uint32_t usage)
    {
        format        = this->getSizedFormat(format);
        bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;

        DkImageFormat color;
        bool supported = deko3d::getConstant(format, color);

        return readable && supported;
    }

    void Graphics::setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                            bool hasSRGBTexture)
    {
        const auto& state   = this->states.back();
        const bool isWindow = targets.getFirstTarget().texture == nullptr;

        if (isWindow)
            d3d.bindFramebuffer();
        else
            d3d.bindFramebuffer((dk::Image*)targets.getFirstTarget().texture->getRenderTargetHandle());

        d3d.setViewport({ 0, 0, pixelWidth, pixelHeight });

        if (state.scissor)
            d3d.setScissor(state.scissorRect);
    }

    TextureBase* Graphics::newTexture(const TextureBase::Settings& settings, const TextureBase::Slices* data)
    {
        return new Texture(this, settings, data);
    }

    FontBase* Graphics::newFont(Rasterizer* data)
    {
        return new Font(data, this->states.back().defaultSamplerState);
    }

    FontBase* Graphics::newDefaultFont(int size, const Rasterizer::Settings& settings)
    {
        auto* fontModule = Module::getInstance<FontModuleBase>(Module::M_FONT);

        if (!fontModule)
            throw love::Exception("Font module has not been loaded.");

        StrongRef<Rasterizer> r(fontModule->newTrueTypeRasterizer(size, settings), Acquire::NO_RETAIN);
        return this->newFont(r.get());
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
        d3d.initialize();

        this->created = true;
        this->initCapabilities();

        try
        {
            // clang-format off
            if (this->batchedDrawState.vertexBuffer == nullptr)
            {
                this->batchedDrawState.indexBuffer  = createStreamBuffer(BUFFERUSAGE_INDEX, INIT_INDEX_BUFFER_SIZE);
                this->batchedDrawState.vertexBuffer = createStreamBuffer(BUFFERUSAGE_VERTEX, INIT_VERTEX_BUFFER_SIZE);
            }
            // clang-format on
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
            const auto type = (Shader::StandardShader)index;
            if (!Shader::standardShaders[index])
            {
                std::vector<std::string> stages {};
                Shader::CompileOptions options {};

                stages.push_back(Shader::getDefaultStagePath(type, SHADERSTAGE_VERTEX));
                stages.push_back(Shader::getDefaultStagePath(type, SHADERSTAGE_PIXEL));

                try
                {
                    Shader::standardShaders[index] = this->newShader(stages, options);
                }
                catch (love::Exception&)
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
        Volatile::unloadAll();

        this->created = false;
    }

    void Graphics::draw(const DrawIndexedCommand& command)
    {
        d3d.prepareDraw(this);
        d3d.setVertexAttributes(command.texture != nullptr);
        d3d.bindTextureToUnit(command.texture, 0);
        d3d.setCullMode(command.cullMode);

        DkPrimitive primitive = DkPrimitive(-1);
        if (!deko3d::getConstant(command.primitiveType, primitive))
            throw love::Exception("Invalid primitive type {:d}.", (int)command.primitiveType);

        const auto indexCount    = command.indexCount;
        const auto offset        = command.indexBufferOffset;
        const auto instanceCount = command.instanceCount;

        d3d.drawIndexed(primitive, indexCount, BUFFER_OFFSET(offset), instanceCount);
        ++drawCalls;
    }

    void Graphics::draw(const DrawCommand& command)
    {
        d3d.prepareDraw(this);
        d3d.setVertexAttributes(command.texture != nullptr);
        d3d.bindTextureToUnit(command.texture, 0);
        d3d.setCullMode(command.cullMode);

        DkPrimitive primitive = DkPrimitive(-1);
        if (!deko3d::getConstant(command.primitiveType, primitive))
            throw love::Exception("Invalid primitive type {:d}.", (int)command.primitiveType);

        d3d.draw(primitive, command.vertexCount, command.vertexStart);
        ++drawCalls;
    }
} // namespace love
