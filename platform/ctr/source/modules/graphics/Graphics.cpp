#include "driver/display/citro3d.hpp"

#include "modules/graphics/Buffer.hpp"
#include "modules/graphics/Graphics.hpp"
#include "modules/graphics/Shader.hpp"
#include "modules/graphics/ShaderStage.hpp"
#include "modules/window/Window.hpp"

#include "modules/graphics/Font.hpp"

#include "common/Exception.hpp"
#include "common/debug.hpp"

namespace love
{
    Graphics::Graphics() : GraphicsBase("love.graphics.citro3d")
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
        this->resetProjection();
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

        if (stencil.hasValue)
            c3d.clearStencil(stencil.value);

        if (depth.hasValue)
            c3d.clearDepth(depth.value);

        if (color.hasValue)
        {
            gammaCorrectColor(color.value);
            c3d.clear(color.value);
        }

        if (!this->isRenderTargetActive())
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

        if (!this->isRenderTargetActive())
            c3d.bindFramebuffer(c3d.getInternalBackbuffer());
    }

    void Graphics::present(void* screenshotCallbackData)
    {
        if (!this->isActive())
            return;

        if (this->isRenderTargetActive())
            throw love::Exception("present cannot be called while a render target is active.");

        c3d.present();

        if (!this->pendingScreenshotCallbacks.empty())
        {
            u16 width, height;
            const auto* top     = gfxGetFramebuffer(GFX_TOP, GFX_LEFT, &width, &height);
            auto* imageModule   = Module::getInstance<Image>(Module::M_IMAGE);
            uint8_t* screenshot = nullptr;

            size_t row  = 4 * width;
            size_t size = row * height;

            try
            {
                screenshot = new uint8_t[size];
            }
            catch (std::exception&)
            {
                delete[] screenshot;
                throw love::Exception(E_OUT_OF_MEMORY);
            }

            std::memcpy(screenshot, top, size);

            for (int index = 0; index < (int)this->pendingScreenshotCallbacks.size(); index++)
            {
                const auto& info = this->pendingScreenshotCallbacks[index];
                ImageData* image = nullptr;

                try
                {
                    image = imageModule->newImageData(width, height, PIXELFORMAT_RGBA8_UNORM, screenshot);
                }
                catch (love::Exception&)
                {
                    delete[] screenshot;
                    info.callback(&info, nullptr, nullptr);
                    for (int j = index + 1; j < (int)this->pendingScreenshotCallbacks.size(); j++)
                    {
                        const auto& nextInfo = this->pendingScreenshotCallbacks[j];
                        nextInfo.callback(&nextInfo, nullptr, nullptr);
                    }
                    this->pendingScreenshotCallbacks.clear();
                    throw;
                }
                info.callback(&info, image, screenshotCallbackData);
                image->release();
            }
            delete[] screenshot;
            this->pendingScreenshotCallbacks.clear();
        }

        c3d.bindFramebuffer(c3d.getInternalBackbuffer());

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

    void Graphics::setStencilState(const StencilState& state)
    {
        Graphics::flushBatchedDraws();
        c3d.setStencilState(state);

        this->states.back().stencil = state;
    }

    void Graphics::setDepthMode(CompareMode compare, bool write)
    {
        auto& state = this->states.back();
        if (state.depthTest != compare || state.depthWrite != write)
            Graphics::flushBatchedDraws();

        state.depthTest  = compare;
        state.depthWrite = write;

        c3d.setDepthWrites(compare, write);
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

        StrongRef<Rasterizer> r(module->newTrueTypeRasterizer(size, settings), Acquire::NO_RETAIN);
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
        c3d.initialize();

        this->created = true;
        this->initCapabilities();

        c3d.setupContext();

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

        this->createQuadIndexBuffer();
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
        const auto& state   = this->states.back();
        const auto isWindow = targets.getFirstTarget().texture == nullptr;

        if (isWindow)
            c3d.bindFramebuffer(c3d.getInternalBackbuffer());
        else
            c3d.bindFramebuffer((C3D_RenderTarget*)targets.getFirstTarget().texture->getRenderTargetHandle());

        bool tilt = isWindow ? true : false;
        c3d.setViewport({ 0, 0, pixelWidth, pixelHeight }, tilt);

        if (state.scissor)
            c3d.setScissor(state.scissorRect);
    }

    void Graphics::setViewport(int x, int y, int width, int height)
    {
        c3d.setViewport({ x, y, width, height }, true);
    }

    TextureBase* Graphics::newTexture(const TextureBase::Settings& settings, const TextureBase::Slices* data)
    {
        return new Texture(this, settings, data);
    }

    BufferBase* Graphics::newBuffer(const Buffer::Settings& settings,
                                    const std::vector<Buffer::DataDeclaration>& format, const void* data,
                                    size_t size, size_t arraylength)
    {
        return new Buffer(this, settings, format, data, size, arraylength);
    }

    void Graphics::points(Vector2* positions, const Color* colors, int count)
    {
        const auto pointSize = this->states.back().pointSize;
        auto points          = this->calculateEllipsePoints(pointSize, pointSize);

        float twoPi = float(LOVE_M_PI * 2);

        if (points <= 0)
            points = 1;

        float shift = twoPi / points;

        // 1 extra point at the end for a closed loop, and 1 extra point at the
        // start in filled mode for the vertex in the center of the ellipse.
        int extraPoints = 2;

        const Matrix4& transform = this->getTransform();
        bool is2D                = transform.isAffine2DTransform();

        std::vector<Vector2> coordinates;
        coordinates.reserve(count * (points + extraPoints));

        for (int point = 0; point < count; point++)
        {
            auto* polygonCoords = this->getScratchBuffer<Vector2>(points + extraPoints);
            float phi           = 0.0f; // Reset phi for each point

            polygonCoords[0].x = positions[point].x;
            polygonCoords[0].y = positions[point].y;

            for (int index = 0; index < points; ++index, phi += shift)
            {
                polygonCoords[index + 1].x = positions[point].x + pointSize * cosf(phi);
                polygonCoords[index + 1].y = positions[point].y + pointSize * sinf(phi);
            }

            polygonCoords[points + 1] = polygonCoords[1]; // Close the shape
            coordinates.insert(coordinates.end(), polygonCoords, polygonCoords + points + extraPoints);
        }

        BatchedDrawCommand command {};
        command.indexMode   = TRIANGLEINDEX_FAN;
        command.format      = CommonFormat::XYf_STf_RGBAf;
        command.vertexCount = coordinates.size();

        BatchedVertexData data = this->requestBatchedDraw(command);

        XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            transform.transformXY(stream, coordinates.data(), command.vertexCount);

        if (!colors)
        {
            Color color = this->getColor();

            for (int index = 0; index < command.vertexCount; index++)
                stream[index].color = color;

            return;
        }

        Color color = this->getColor();
        gammaCorrectColor(color);

        for (int point = 0; point < count; point++)
        {
            Color pointColor = colors ? colors[point] : color;

            if (isGammaCorrect())
            {
                gammaCorrectColor(pointColor);
                pointColor *= color;
                unGammaCorrectColor(pointColor);
            }

            int vertexOffset = point * (points + extraPoints);
            for (int index = 0; index < points + extraPoints; index++)
                stream[vertexOffset + index].color = pointColor;
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
        VertexAttributes attributes {};
        this->findVertexAttributes(command.attributesID, attributes);

        c3d.prepareDraw(this);
        c3d.setTexEnvMode(command.texture, command.isFont);
        c3d.setVertexAttributes(attributes, *command.buffers);
        c3d.bindTextureToUnit(command.texture, 0);

        const auto* indices = (const uint16_t*)command.indexBuffer->getHandle();
        const int index     = BUFFER_OFFSET(command.indexBufferOffset);

        const auto primitiveType = citro3d::getPrimitiveType(command.primitiveType);
        const auto dataType      = C3D_UNSIGNED_SHORT;

        C3D_DrawElements(primitiveType, command.indexCount, dataType, &indices[index]);
        ++this->drawCalls;
    }

    void Graphics::draw(const DrawCommand& command)
    {
        VertexAttributes attributes {};
        this->findVertexAttributes(command.attributesID, attributes);

        c3d.prepareDraw(this);
        c3d.setTexEnvMode(command.texture, command.isFont);
        c3d.setVertexAttributes(attributes, *command.buffers);
        c3d.bindTextureToUnit(command.texture, 0);

        const auto primitiveType = citro3d::getPrimitiveType(command.primitiveType);

        C3D_DrawArrays(primitiveType, command.vertexStart, command.vertexCount);
        ++this->drawCalls;
    }

    static void advanceVertexOffsets(const VertexAttributes& attributes, BufferBindings& buffers, int count)
    {
        uint32_t touchedBuffers = 0;
        for (size_t index = 0; index < VertexAttributes::MAX; index++)
        {
            if (!attributes.isEnabled(index))
                continue;

            auto& attribute    = attributes.attributes[index];
            uint32_t bufferBit = 1u << attribute.bufferIndex;
            if ((touchedBuffers & bufferBit) == 0)
            {
                touchedBuffers |= bufferBit;
                const auto layout = attributes.bufferLayouts[attribute.bufferIndex];
                buffers.info[attribute.bufferIndex].offset += layout.stride * count;
            }
        }
    }

    void Graphics::drawQuads(int start, int count, VertexAttributesID attributesID,
                             const BufferBindings& buffers, TextureBase* texture)
    {
        VertexAttributes attributes {};
        this->findVertexAttributes(attributesID, attributes);

        c3d.prepareDraw(this);
        c3d.bindTextureToUnit(texture, 0);
        c3d.setCullMode(CULL_NONE);

        const auto* indices = (uint16_t*)this->quadIndexBuffer->getHandle();

        BufferBindings buffersCopy = buffers;
        if (start > 0)
            advanceVertexOffsets(attributes, buffersCopy, start * 4);

        for (int quadIndex = 0; quadIndex < count; quadIndex += MAX_QUADS_PER_DRAW)
        {
            c3d.setVertexAttributes(attributes, buffersCopy);
            int quadCount = std::min(MAX_QUADS_PER_DRAW, count - quadIndex);
            C3D_DrawElements(GPU_TRIANGLES, quadCount * 6, C3D_UNSIGNED_SHORT, indices);
            ++drawCalls;

            if (count > MAX_QUADS_PER_DRAW)
                advanceVertexOffsets(attributes, buffersCopy, quadCount * 4);
        }
    }

    bool Graphics::isStereoscopic() const
    {
        return gfxIs3D();
    }

    void Graphics::setStereoscopic(bool enable)
    {
        c3d.set3DMode(enable);
    }

    bool Graphics::isWideMode() const
    {
        return gfxIsWide();
    }

    void Graphics::setWideMode(bool enable)
    {
        c3d.setWideMode(enable);
    }

    float Graphics::getDepth() const
    {
        return osGet3DSliderState();
    }
} // namespace love
