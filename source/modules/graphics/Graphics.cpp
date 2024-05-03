#include "modules/graphics/Graphics.tcc"

#include "common/Console.hpp"

namespace love
{
    GraphicsBase::GraphicsBase(const char* name) :
        Module(M_GRAPHICS, name),
        width(0),
        height(0),
        pixelWidth(0),
        pixelHeight(0),
        created(false),
        active(true),
        deviceProjectionMatrix(),
        batchedDrawState()
    {
        this->transformStack.reserve(16);
        this->transformStack.push_back(Matrix4());

        this->pixelScaleStack.reserve(16);
        this->pixelScaleStack.push_back(1.0);

        this->states.reserve(10);
        this->states.push_back(DisplayState());
    }

    GraphicsBase::~GraphicsBase()
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

        if (this->batchedDrawState.vertexBuffer)
            this->batchedDrawState.vertexBuffer->release();

        if (this->batchedDrawState.indexBuffer)
            this->batchedDrawState.indexBuffer->release();
    }

    void GraphicsBase::restoreState(const DisplayState& state)
    {
        this->setColor(state.color);
        this->setBackgroundColor(state.backgroundColor);

        this->setBlendState(state.blend);

        this->setLineWidth(state.lineWidth);
        this->setLineStyle(state.lineStyle);
        this->setLineJoin(state.lineJoin);

        this->setPointSize(state.pointSize);

        if (state.scissor)
            this->setScissor(state.scissorRect);
        else
            this->setScissor();

        this->setMeshCullMode(state.meshCullMode);
        this->setFrontFaceWinding(state.winding);

        // this->setFont(state.font.get());
        // this->setShader(state.shader.get());
        // this->setRenderTargets(state.renderTargets);

        // this->setStencilState(state.stencil);
        // this->setDepthMode(state.depthTest, state.depthWrite);

        this->setColorMask(state.colorMask);

        this->setDefaultSamplerState(state.defaultSampleState);

        if (state.useCustomProjection)
            this->updateDeviceProjection(state.customProjection);
        else
            this->resetProjection();
    }

    void GraphicsBase::restoreStateChecked(const DisplayState& state)
    {
        const auto& current = this->states.back();

        if (state.color != current.color)
            this->setColor(state.color);

        this->setBackgroundColor(state.backgroundColor);

        if (state.blend != current.blend)
            this->setBlendState(state.blend);

        this->setLineWidth(state.lineWidth);
        this->setLineStyle(state.lineStyle);
        this->setLineJoin(state.lineJoin);

        if (state.pointSize != current.pointSize)
            this->setPointSize(state.pointSize);

        if (state.scissor != current.scissor ||
            (state.scissor && !(state.scissorRect != current.scissorRect)))
        {
            if (state.scissor)
                this->setScissor(state.scissorRect);
            else
                this->setScissor();
        }

        this->setMeshCullMode(state.meshCullMode);

        if (state.winding != current.winding)
            this->setFrontFaceWinding(state.winding);

        // this->setFont(state.font.get());
        // this->setShader(state.shader.get());

        // if (this->stencil != state.stencil)
        //     this->setStencilState(state.stencil);

        // if (this->depthTest != state.depthTest || this->depthWrite != state.depthWrite)
        //     this->setDepthMode(state.depthTest, state.depthWrite);

        if (state.colorMask != current.colorMask)
            this->setColorMask(state.colorMask);

        this->setDefaultSamplerState(state.defaultSampleState);

        if (state.useCustomProjection)
            this->updateDeviceProjection(state.customProjection);
        else if (current.useCustomProjection)
            this->resetProjection();
    }

    void GraphicsBase::setScissor(const Rect& scissor)
    {
        this->flushBatchedDraws();

        auto& state = this->states.back();

        Rect rect {};
        rect.x = scissor.x;
        rect.y = scissor.y;
        rect.w = std::max(0, scissor.w);
        rect.h = std::max(0, scissor.h);

        state.scissorRect = rect;
        state.scissor     = true;
    }

    void GraphicsBase::setScissor()
    {
        if (this->states.back().scissor)
            this->flushBatchedDraws();

        this->states.back().scissor = false;
    }

    GraphicsBase::Stats GraphicsBase::getStats() const
    {
        Stats stats {};

        stats.drawCalls = this->drawCalls;
        if (this->batchedDrawState.vertexCount > 0)
            stats.drawCalls++;

        stats.drawCallsBatched = this->drawCallsBatched;
        stats.textures         = TextureBase::textureCount;
        stats.textureMemory    = TextureBase::totalGraphicsMemory;

        return stats;
    }

    void GraphicsBase::setFrontFaceWinding(Winding winding)
    {
        if (this->states.back().winding != winding)
            this->flushBatchedDraws();

        this->states.back().winding = winding;
    }

    Winding GraphicsBase::getFrontFaceWinding() const
    {
        return this->states.back().winding;
    }

    void GraphicsBase::setColorMask(ColorChannelMask mask)
    {
        this->flushBatchedDraws();
        this->states.back().colorMask = mask;
    }

    ColorChannelMask GraphicsBase::getColorMask() const
    {
        return this->states.back().colorMask;
    }

    void GraphicsBase::setBlendMode(BlendMode mode, BlendAlpha alphaMode)
    {
        if (alphaMode == BLENDALPHA_MULTIPLY && !isAlphaMultiplyBlendSupported(mode))
        {
            std::string_view modeName = "unknown";
            love::getConstant(mode, modeName);

            throw love::Exception("The '{}' blend mode must be used with premultiplied alpha.", modeName);
        }

        this->setBlendState(computeBlendState(mode, alphaMode));
    }

    void GraphicsBase::setBlendState(const BlendState& state)
    {
        if (!(state == this->states.back().blend))
            this->flushBatchedDraws();

        this->states.back().blend = state;
    }

    BatchedVertexData GraphicsBase::requestBatchDraw(const BatchedDrawCommand& command)
    {
        BatchedDrawState& state = this->batchedDrawState;

        bool shouldFlush  = false;
        bool shouldResize = false;

        // clang-format off
        if (command.primitiveMode != state.primitiveMode
            || command.format != state.format
            || ((command.indexMode != TRIANGLEINDEX_NONE) != (state.indexCount > 0))
            || command.texture != state.texture
            || command.shaderType != state.shaderType)
        {
            shouldFlush = true;
        }
        // clang-format on

        int totalVertices = state.vertexCount + command.vertexCount;

        if (totalVertices > LOVE_UINT16_MAX && command.indexMode != TRIANGLEINDEX_NONE)
            shouldFlush = true;

        int requestedIndexCount   = getIndexCount(command.indexMode, command.vertexCount);
        size_t requestedIndexSize = requestedIndexCount * sizeof(uint16_t);

        size_t newDataSize    = 0;
        size_t bufferSizes[2] = { 0, 0 };

        if (command.format != CommonFormat::NONE)
        {
            size_t stride   = getFormatStride(command.format);
            size_t dataSize = stride * totalVertices;

            if (state.vertexMap.data != nullptr && dataSize > state.vertexMap.size)
                shouldFlush = true;

            if (dataSize > state.vertexBuffer->getUsableSize())
            {
                bufferSizes[0] = std::max(dataSize, state.vertexBuffer->getSize() * 2);
                shouldResize   = true;
            }

            newDataSize = stride * command.vertexCount;
        }

        if (command.indexMode != TRIANGLEINDEX_NONE)
        {
            size_t dataSize = (state.indexCount + requestedIndexCount) * sizeof(uint16_t);

            if (state.indexMap.data != nullptr && dataSize > state.indexMap.size)
                shouldFlush = true;

            if (dataSize > state.indexBuffer->getUsableSize())
            {
                bufferSizes[1] = std::max(dataSize, state.indexBuffer->getSize() * 2);
                shouldResize   = true;
            }
        }

        if (shouldFlush || shouldResize)
        {
            flushBatchedDraws();

            state.primitiveMode = command.primitiveMode;
            state.format        = command.format;
            state.texture       = command.texture;
            state.shaderType    = command.shaderType;
        }

        if (state.vertexCount == 0)
        {
            if (ShaderBase::isDefaultActive())
                ShaderBase::attachDefault(state.shaderType);
        }

        if (shouldResize)
        {
            if (state.vertexBuffer->getSize() < bufferSizes[0])
            {
                state.vertexBuffer->release();
                state.vertexBuffer = new StreamBuffer(BUFFERUSAGE_VERTEX, bufferSizes[0]);
            }

            if (state.indexBuffer->getSize() < bufferSizes[1])
            {
                state.indexBuffer->release();
                state.indexBuffer = new StreamBuffer(BUFFERUSAGE_INDEX, bufferSizes[1]);
            }
        }

        if (command.indexMode != TRIANGLEINDEX_NONE)
        {
            if (state.indexMap.data == nullptr)
                state.indexMap = state.indexBuffer->map();

            uint16_t* indices = (uint16_t*)state.indexMap.data;
            fillIndices(command.indexMode, state.vertexCount, command.vertexCount, indices);

            state.indexMap.data += requestedIndexSize;
        }

        BatchedVertexData data {};

        if (newDataSize > 0)
        {
            if (state.vertexMap.data == nullptr)
                state.vertexMap = state.vertexBuffer->map();

            data.stream = state.vertexMap.data;
            state.vertexMap.data += newDataSize;
        }

        if (state.vertexCount > 0)
            this->drawCallsBatched++;

        state.vertexCount += command.vertexCount;
        state.indexCount += requestedIndexCount;

        return data;
    }

    void GraphicsBase::flushBatchedDraws()
    {
        BatchedDrawState& state = this->batchedDrawState;

        if ((state.vertexCount == 0 && state.indexCount == 0) || state.flushing)
            return;

        size_t usedSizes[2] = { 0, 0 };

        if (state.format != CommonFormat::NONE)
        {
            usedSizes[0] = getFormatStride(state.format) * state.vertexCount;

            auto offset = state.vertexBuffer->unmap(usedSizes[0]);
            state.vertexBuffer->bind((const void*)offset);

            state.vertexMap = StreamBuffer::MapInfo();
        }

        state.flushing = true;

        this->pushIdentityTransform();

        if (state.indexCount > 0)
        {
            usedSizes[1] = sizeof(uint16_t) * state.indexCount;

            DrawIndexedCommand command {};
            command.primitiveType     = state.primitiveMode;
            command.indexCount        = state.indexCount;
            command.indexType         = INDEX_UINT16;
            command.indexBufferOffset = state.indexBuffer->unmap(usedSizes[1]);
            command.texture           = state.texture;
            this->draw(command);

            state.indexMap = StreamBuffer::MapInfo();
        }

        if (usedSizes[0] > 0)
            state.vertexBuffer->markUsed(usedSizes[0]);

        if (usedSizes[1] > 0)
            state.indexBuffer->markUsed(usedSizes[1]);

        this->popTransform();

        state.vertexCount = 0;
        state.indexCount  = 0;
        state.flushing    = false;
    }

    void GraphicsBase::flushBatchedDrawsGlobal()
    {
        auto* instance = Module::getInstance<GraphicsBase>(Module::M_GRAPHICS);

        if (instance != nullptr)
            instance->flushBatchedDraws();
    }

    TextureBase* GraphicsBase::getDefaultTexture(TextureBase* texture)
    {
        if (texture != nullptr)
            return texture;

        return getDefaultTexture(TEXTURE_2D);
    }

    TextureBase* GraphicsBase::getDefaultTexture(TextureType type)
    {
        TextureBase* texture = this->defaultTextures[type];

        if (texture != nullptr)
            return texture;

        TextureBase::Settings settings {};
        settings.type = type;

        if constexpr (Console::is(Console::CTR))
        {
            settings.width  = 5;
            settings.height = 5;
        }

        texture = this->newTexture(settings);

        SamplerState state {};
        state.minFilter = state.magFilter = SamplerState::FILTER_NEAREST;
        state.wrapU = state.wrapV = state.wrapW = SamplerState::WRAP_CLAMP;

        texture->setSamplerState(state);

        uint8_t pixel[4] = { 255, 255, 255, 255 };
        if (isPixelFormatInteger(settings.format))
            pixel[0] = pixel[1] = pixel[2] = pixel[3] = 1;

        // clang-format off
        for (int slice = 0; slice < (type == TEXTURE_CUBE ? 6 : 1); slice++)
            texture->replacePixels(pixel, sizeof(pixel), slice, 0, { 0, 0, settings.width + 1, settings.height + 1 }, false);
        // clang-format on

        this->defaultTextures[type] = texture;

        return texture;
    }

    void GraphicsBase::polyline(const std::span<Vector2> vertices)
    {}

    void GraphicsBase::polygon(DrawMode mode, std::span<Vector2> vertices, bool skipLastVertex)
    {
        if (mode == DRAW_LINE)
            this->polyline(vertices);
        else
        {
            const auto& transform = this->getTransform();
            bool is2D             = transform.isAffine2DTransform();

            BatchedDrawCommand command {};
            command.format      = CommonFormat::XYf_STf_RGBAf;
            command.indexMode   = TRIANGLEINDEX_FAN;
            command.vertexCount = (int)vertices.size() - (skipLastVertex ? 1 : 0);

            BatchedVertexData data = this->requestBatchDraw(command);

            constexpr float inf = std::numeric_limits<float>::infinity();
            Vector2 minimum(inf, inf);
            Vector2 maximum(-inf, -inf);

            for (int index = 0; index < command.vertexCount; index++)
            {
                Vector2 vector = vertices[index];
                minimum.x      = std::min(minimum.x, vector.x);
                minimum.y      = std::min(minimum.y, vector.y);
                maximum.x      = std::max(maximum.x, vector.x);
                maximum.y      = std::max(maximum.y, vector.y);
            }

            Vector2 invSize(1.0f / (maximum.x - minimum.x), 1.0f / (maximum.y - minimum.y));
            Vector2 start(minimum.x * invSize.x, minimum.y * invSize.y);

            XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

            for (int index = 0; index < command.vertexCount; index++)
            {
                stream[index].s     = vertices[index].x * invSize.x - start.x;
                stream[index].t     = vertices[index].y * invSize.y - start.y;
                stream[index].color = this->getColor();
            }

            if (is2D)
                transform.transformXY(stream, vertices.data(), command.vertexCount);
        }
    }

    int GraphicsBase::calculateEllipsePoints(float a, float b) const
    {
        auto points = (int)std::sqrt(((a + b) / 2.0f) * 20.0f * (float)this->pixelScaleStack.back());

        return std::max(points, 8);
    }

    void GraphicsBase::rectangle(DrawMode mode, float x, float y, float w, float h)
    {
        // clang-format off
        std::array<Vector2, 5> coords = { Vector2(x, y), Vector2(x, y + h), Vector2(x + w, y + h), Vector2(x + w, y), Vector2(x, y) };
        // clang-format on

        this->polygon(mode, coords);
    }

    void GraphicsBase::rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry,
                                 int points)
    {
        if (rx <= 0 || ry <= 0)
            return this->rectangle(mode, x, y, w, h);

        if (w >= 0.02f)
            rx = std::min(rx, w / 2.0f - 0.01f);

        if (h >= 0.02f)
            ry = std::min(ry, h / 2.0f - 0.01f);

        points = std::max(points / 4, 1);

        const float halfPi = float(LOVE_M_PI / 2);
        float shift        = halfPi / ((float)points + 1.0f);

        size_t numCoords = (points + 2) * 4;
        auto* coords     = this->getScratchBuffer<Vector2>(numCoords + 1);

        float phi = 0.0f;

        for (int i = 0; i <= points + 2; ++i, phi += shift)
        {
            coords[i].x = x + rx * (1 - cosf(phi));
            coords[i].y = y + ry * (1 - sinf(phi));
        }

        phi = halfPi;

        for (int i = points + 2; i <= 2 * (points + 2); ++i, phi += shift)
        {
            coords[i].x = x + w - rx * (1 + cosf(phi));
            coords[i].y = y + ry * (1 - sinf(phi));
        }

        phi = 2 * halfPi;

        for (int i = 2 * (points + 2); i <= 3 * (points + 2); ++i, phi += shift)
        {
            coords[i].x = x + w - rx * (1 + cosf(phi));
            coords[i].y = y + h - ry * (1 + sinf(phi));
        }

        phi = 3 * halfPi;

        for (int i = 3 * (points + 2); i <= 4 * (points + 2); ++i, phi += shift)
        {
            coords[i].x = x + rx * (1 - cosf(phi));
            coords[i].y = y + h - ry * (1 + sinf(phi));
        }

        coords[numCoords] = coords[0];
        this->polygon(mode, std::span(coords, numCoords + 1));
    }

    void GraphicsBase::rectangle(DrawMode mode, float x, float y, float w, float h, float rx, float ry)
    {
        const float a = std::min(rx, std::abs(w / 2));
        const float b = std::min(ry, std::abs(h / 2));

        int points = this->calculateEllipsePoints(a, b);
        this->rectangle(mode, x, y, w, h, rx, ry, points);
    }

    void GraphicsBase::ellipse(DrawMode mode, float x, float y, float a, float b, int points)
    {
        float twoPi = float(LOVE_M_PI * 2);

        if (points <= 0)
            points = 1;

        float shift = twoPi / points;
        float phi   = 0.0f;

        int extraPoints = 1 + (mode == DRAW_FILL ? 1 : 0);

        auto* polygonCoords = this->getScratchBuffer<Vector2>(points + extraPoints);
        auto* coords        = polygonCoords;

        if (mode == DRAW_FILL)
        {
            coords[0].x = x;
            coords[0].y = y;
            coords++;
        }

        for (int index = 0; index < points; ++index, phi += shift)
        {
            coords[index].x = x + a * std::cos(phi);
            coords[index].y = y + b * std::sin(phi);
        }

        coords[points] = coords[0];
        this->polygon(mode, std::span(polygonCoords, points + extraPoints), false);
    }

    void GraphicsBase::ellipse(DrawMode mode, float x, float y, float a, float b)
    {
        int points = this->calculateEllipsePoints(a, b);
        this->ellipse(mode, x, y, a, b, points);
    }

    void GraphicsBase::circle(DrawMode mode, float x, float y, float radius, int points)
    {
        this->ellipse(mode, x, y, radius, radius, points);
    }

    void GraphicsBase::circle(DrawMode mode, float x, float y, float radius)
    {
        this->ellipse(mode, x, y, radius, radius);
    }

    void GraphicsBase::arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                           float angle2, int points)
    {
        if (points <= 0 || angle1 == angle2)
            return;

        if (std::abs(angle1 - angle2) >= 2.0f * (float)LOVE_M_PI)
        {
            this->circle(mode, x, y, radius, points);
            return;
        }

        float shift = (angle2 - angle1) / points;

        if (shift == 0.0)
            return;

        if (mode == DRAW_LINE && arcMode == ARC_CLOSED && std::abs(angle1 - angle2) < LOVE_TORAD(4))
            arcMode = ARC_OPEN;

        if (mode == DRAW_FILL && arcMode == ARC_OPEN)
            arcMode = ARC_CLOSED;

        float phi = angle1;

        Vector2* coords = nullptr;
        int numCoords   = 0;

        const auto createPoints = [&](Vector2* coordinates) {
            for (int i = 0; i <= points; ++i, phi += shift)
            {
                coordinates[i].x = x + radius * cosf(phi);
                coordinates[i].y = y + radius * sinf(phi);
            }
        };

        if (arcMode == ARC_PIE)
        {
            numCoords = points + 3;

            coords    = this->getScratchBuffer<Vector2>(numCoords);
            coords[0] = coords[numCoords - 1] = Vector2(x, y);

            createPoints(coords + 1);
        }
        else if (arcMode == ARC_OPEN)
        {
            numCoords = points + 1;
            coords    = this->getScratchBuffer<Vector2>(numCoords);

            createPoints(coords);
        }
        else
        {
            numCoords = points + 2;
            coords    = this->getScratchBuffer<Vector2>(numCoords);

            createPoints(coords);
            coords[numCoords - 1] = coords[0];
        }

        this->polygon(mode, std::span(coords, numCoords));
    }

    void GraphicsBase::arc(DrawMode mode, ArcMode arcMode, float x, float y, float radius, float angle1,
                           float angle2)
    {
        float points = (float)this->calculateEllipsePoints(radius, radius);
        float angle  = std::abs(angle1 - angle2);

        if (angle < 2.0f * (float)LOVE_M_PI)
            points *= angle / (2.0f * (float)LOVE_M_PI);

        this->arc(mode, arcMode, x, y, radius, angle1, angle2, int(points + 0.5f));
    }

    void GraphicsBase::points(const Vector2* positions, const Color* colors, int count)
    {
        const Matrix4& transform = this->getTransform();
        bool is2D                = transform.isAffine2DTransform();

        BatchedDrawCommand command {};
        command.primitiveMode = PRIMITIVE_POINTS;
        command.format        = CommonFormat::XYf_STf_RGBAf;
        command.vertexCount   = count;

        BatchedVertexData data = this->requestBatchDraw(command);

        XYf_STf_RGBAf* stream = (XYf_STf_RGBAf*)data.stream;

        if (is2D)
            transform.transformXY(stream, positions, command.vertexCount);

        if (!colors)
        {
            Color color = this->getColor();

            for (int index = 0; index < command.vertexCount; index++)
                stream[index].color = color;

            return;
        }

        Color color = this->getColor();
        gammaCorrectColor(color);

        if (isGammaCorrect())
        {
            for (int index = 0; index < command.vertexCount; index++)
            {
                Color current = colors[index];

                gammaCorrectColor(current);
                current *= color;
                unGammaCorrectColor(current);

                stream[index].color = current;
            }
        }
        else
        {
            for (int index = 0; index < command.vertexCount; index++)
                stream[index].color = colors[index];
        }
    }

    void GraphicsBase::draw(Drawable* drawable, const Matrix4& matrix)
    {
        drawable->draw(this, matrix);
    }

    void GraphicsBase::draw(TextureBase* texture, Quad* quad, const Matrix4& matrix)
    {
        texture->draw(this, quad, matrix);
    }
} // namespace love
