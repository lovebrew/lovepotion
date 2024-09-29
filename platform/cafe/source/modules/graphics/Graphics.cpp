#include "driver/display/GX2.hpp"

#include "modules/graphics/Graphics.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/Font.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/graphics/Texture.hpp"

#include <gx2/draw.h>
#include <gx2r/draw.h>

namespace love
{
    Graphics::Graphics() : GraphicsBase("love.graphics.gx2")
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

    void Graphics::captureScreenshot(const ScreenshotInfo& info)
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
        this->capabilities.limits[LIMIT_TEXTURE_SIZE]               = 4096;
        this->capabilities.limits[LIMIT_TEXTURE_LAYERS]             = 1;
        this->capabilities.limits[LIMIT_VOLUME_TEXTURE_SIZE]        = 4096;
        this->capabilities.limits[LIMIT_CUBE_TEXTURE_SIZE]          = 4096;
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
        gx2.ensureInFrame();
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
            gx2.clear(color.value);
        }

        gx2.bindFramebuffer(&gx2.getInternalBackbuffer());
    }

    GX2ColorBuffer Graphics::getInternalBackbuffer() const
    {
        return gx2.getInternalBackbuffer();
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
            gx2.clear(value);
        }

        gx2.bindFramebuffer(&gx2.getInternalBackbuffer());
    }

    void Graphics::present(void* screenshotCallbackData)
    {
        if (!this->isActive())
            return;

        if (this->isRenderTargetActive())
            throw love::Exception("present cannot be called while a render target is active.");

        gx2.present();

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

        gx2.setScissor(rectangle);

        state.scissor     = true;
        state.scissorRect = scissor;
    }

    void Graphics::setScissor()
    {
        if (this->states.back().scissor)
            this->flushBatchedDraws();

        this->states.back().scissor = false;
        gx2.setScissor(Rect::EMPTY);
    }

    void Graphics::setFrontFaceWinding(Winding winding)
    {
        auto& state = this->states.back();

        if (state.winding != winding)
            this->flushBatchedDraws();

        state.winding = winding;

        if (this->isRenderTargetActive())
            winding = (winding == WINDING_CW) ? WINDING_CCW : WINDING_CW; // ???

        gx2.setVertexWinding(winding);
    }

    void Graphics::setColorMask(ColorChannelMask mask)
    {
        this->flushBatchedDraws();

        gx2.setColorMask(mask);
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
            gx2.setBlendState(state);

        this->states.back().blend = state;
    }

    bool Graphics::isPixelFormatSupported(PixelFormat format, uint32_t usage)
    {
        format        = this->getSizedFormat(format);
        bool readable = (usage & PIXELFORMATUSAGEFLAGS_SAMPLE) != 0;

        GX2SurfaceFormat color;
        bool supported = GX2::getConstant(format, color);

        return readable && supported;
    }

    void Graphics::setRenderTargetsInternal(const RenderTargets& targets, int pixelWidth, int pixelHeight,
                                            bool hasSRGBTexture)
    {
        const auto& state = this->states.back();

        bool isWindow = targets.getFirstTarget().texture == nullptr;

        if (isWindow)
            gx2.bindFramebuffer(&gx2.getInternalBackbuffer());
        else
            gx2.bindFramebuffer((GX2ColorBuffer*)targets.getFirstTarget().texture->getRenderTargetHandle());

        gx2.setViewport({ 0, 0, pixelWidth, pixelHeight });

        if (state.scissor)
            gx2.setScissor(state.scissorRect);
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
        auto* module = Module::getInstance<FontModuleBase>(Module::M_FONT);

        if (module == nullptr)
            throw love::Exception("Font module has not been loaded.");

        StrongRef<Rasterizer> rasterizer = module->newTrueTypeRasterizer(size, settings);

        return this->newFont(rasterizer.get());
    }

    bool Graphics::setMode(int width, int height, int pixelWidth, int pixelHeight, bool backBufferStencil,
                           bool backBufferDepth, int msaa)
    {
        gx2.initialize();

        this->created = true;
        this->initCapabilities();

        // gx2.setupContext();

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
        gx2.setViewport({ x, y, width, height });
    }

    void Graphics::draw(const DrawIndexedCommand& command)
    {
        gx2.prepareDraw(this);
        gx2.bindTextureToUnit(command.texture, 0);

        const auto mode              = GX2::getPrimitiveType(command.primitiveType);
        auto* buffer                 = (GX2RBuffer*)command.indexBuffer->getHandle();
        const auto indexType         = GX2::getIndexType(command.indexType);
        const uint32_t count         = (uint32_t)command.indexCount;
        const uint32_t offset        = (uint32_t)command.indexBufferOffset;
        const uint32_t instanceCount = (uint32_t)command.instanceCount;

        GX2RDrawIndexed(mode, buffer, indexType, count, offset, 0, instanceCount);
        ++this->drawCalls;
    }

    void Graphics::draw(const DrawCommand& command)
    {}
} // namespace love
