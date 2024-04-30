#include "modules/graphics/Graphics.tcc"

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
        this->states.clear();

        if (this->batchedDrawState.vb[0])
            this->batchedDrawState.vb[0]->release();

        if (this->batchedDrawState.vb[1])
            this->batchedDrawState.vb[1]->release();

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

    GraphicsBase::BatchedVertexData GraphicsBase::requestBatchedDraw(
        const GraphicsBase::BatchedDrawCommand& command)
    {
        BatchedDrawState& state = this->batchedDrawState;

        bool shouldFlush  = false;
        bool shouldResize = false;

        if (command.primitiveMode != state.primitiveMode || command.formats[0] != state.formats[0] ||
            command.formats[1] != state.formats[1] ||
            ((command.indexMode != TRIANGLEINDEX_NONE) != (state.indexCount > 0)) ||
            command.texture != state.texture || command.shaderType != state.shaderType)
        {
            shouldFlush = true;
        }

        int totalVertices = state.vertexCount + command.vertexCount;

        if (totalVertices > LOVE_UINT16_MAX && command.indexMode != TRIANGLEINDEX_NONE)
            shouldFlush = true;

        int requestedIndexCount = getIndexCount(command.indexMode, command.vertexCount);
        size_t requestedSize    = requestedIndexCount * sizeof(uint16_t);

        size_t newDataSizes[2] = { 0, 0 }; //< position, vertices
        size_t bufferSizes[3]  = { 0, 0, 0 };

        for (int index = 0; index < 2; index++)
        {
            if (command.formats[index] == CommonFormat::NONE)
                continue;

            size_t stride   = getFormatStride(command.formats[index]);
            size_t dataSize = stride * totalVertices;

            if (state.vbMap[index].data != nullptr && dataSize > state.vbMap[index].size)
                shouldFlush = true;

            if (dataSize > state.vb[index]->getUsableSize())
            {
                bufferSizes[index] = std::max(dataSize, state.vb[index]->getSize() * 2);
                shouldResize       = true;
            }

            newDataSizes[index] = stride * command.vertexCount;
        }

        if (command.indexMode != TRIANGLEINDEX_NONE)
        {
            size_t dataSize = (state.indexCount + requestedIndexCount) * sizeof(uint16_t);

            if (state.indexBufferMap.data != nullptr && dataSize > state.indexBufferMap.size)
                shouldFlush = true;

            if (dataSize > state.indexBuffer->getUsableSize())
            {
                bufferSizes[2] = std::max(dataSize, state.indexBuffer->getSize() * 2);
                shouldResize   = true;
            }
        }

        if (shouldFlush || shouldResize)
        {
            flushBatchedDraws();

            state.primitiveMode = command.primitiveMode;
            state.formats[0]    = command.formats[0];
            state.formats[1]    = command.formats[1];
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
            for (int index = 0; index < 2; index++)
            {
                if (state.vb[index]->getSize() < bufferSizes[index])
                {
                    state.vb[index]->release();
                    state.vb[index] = createStreamBuffer(BUFFERUSAGE_VERTEX, bufferSizes[index]);
                }
            }

            if (state.indexBuffer->getSize() < bufferSizes[2])
            {
                state.indexBuffer->release();
                state.indexBuffer = createStreamBuffer(BUFFERUSAGE_INDEX, bufferSizes[2]);
            }
        }

        if (command.indexMode != TRIANGLEINDEX_NONE)
        {
            if (state.indexBufferMap.data == nullptr)
                state.indexBufferMap = state.indexBuffer->map(requestedSize);

            uint16_t* indices = (uint16_t*)state.indexBufferMap.data;
            fillIndices(command.indexMode, state.vertexCount, command.vertexCount, indices);

            state.indexBufferMap.data += requestedSize;
        }

        BatchedVertexData data {};

        for (int index = 0; index < 2; index++)
        {
            if (newDataSizes[index] > 0)
            {
                if (state.vbMap[index].data == nullptr)
                    state.vbMap[index] = state.vb[index]->map(newDataSizes[index]);

                data.stream[index] = state.vbMap[index].data;
                state.vbMap[index].data += newDataSizes[index];
            }
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

        VertexAttributes attributes {};
        BufferBindings buffers {};

        size_t usedSizes[3] = { 0, 0, 0 };

        for (int index = 0; index < 2; index++)
        {
            if (state.formats[index] == CommonFormat::NONE)
                continue;

            attributes.setCommonFormat(state.formats[index], (uint8_t)index);

            usedSizes[index] = getFormatStride(state.formats[index]) * state.vertexCount;

            size_t offset = state.vb[index]->unmap(usedSizes[index]);
            buffers.set(index, state.vb[index], offset);

            state.vbMap[index] = StreamBufferBase::MapInfo();
        }

        if (attributes.enableBits == 0)
            return;

        state.flushing = true;

        Color newColor = this->getColor();
        if (attributes.isEnabled(ATTRIB_COLOR))
            this->setColor(Color::WHITE);

        this->pushIdentityTransform();

        if (state.indexCount > 0)
        {
            usedSizes[2] = sizeof(uint16_t) * state.indexCount;

            DrawIndexedCommand command(&attributes, &buffers, state.indexBuffer);
            command.primitiveType     = state.primitiveMode;
            command.indexCount        = state.indexCount;
            command.indexType         = INDEX_UINT16;
            command.indexBufferOffset = state.indexBuffer->unmap(usedSizes[2]);
            command.texture           = nullptr;

            this->draw(command);
            state.indexBufferMap = StreamBufferBase::MapInfo();
        }
        else
        {
            DrawCommand command(&attributes, &buffers);
            command.primitiveType = state.primitiveMode;
            command.vertexCount   = 0;
            command.vertexStart   = state.vertexCount;
            command.texture       = state.texture;

            this->draw(command);
        }

        for (int index = 0; index < 2; index++)
        {
            if (usedSizes[index] > 0)
                state.vb[index]->markUsed(usedSizes[index]);
        }

        if (usedSizes[2] > 0)
            state.indexBuffer->markUsed(usedSizes[2]);

        this->popTransform();

        if (attributes.isEnabled(ATTRIB_COLOR))
            this->setColor(newColor);

        state.vertexCount = 0;
        state.indexCount  = 0;
        state.flushing    = false;
    }

    void GraphicsBase::flushBatchedDrawsGlobal()
    {
        auto* instance = getInstance<GraphicsBase>(M_GRAPHICS);

        if (instance != nullptr)
            instance->flushBatchedDraws();
    }

    void GraphicsBase::setScissor(const Rect& scissor)
    {
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
        this->states.back().winding = winding;
    }

    Winding GraphicsBase::getFrontFaceWinding() const
    {
        return this->states.back().winding;
    }

    void GraphicsBase::setColorMask(ColorChannelMask mask)
    {
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
        this->states.back().blend = state;
    }
} // namespace love
