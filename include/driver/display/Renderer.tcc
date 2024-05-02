#pragma once

#include "common/Singleton.tcc"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/renderstate.hpp"
#include "modules/graphics/vertex.hpp"

#include "driver/graphics/DrawCommand.hpp"

#include <array>
#include <vector>

#define BUFFER_OFFSET(i) ((char*)NULL + (i))

namespace love
{
    template<class T>
    class RendererBase : public Singleton<T>
    {
      public:
        enum RENDERER_INFO
        {
            RENDERER_INFO_NAME,
            RENDERER_INFO_VERSION,
            RENDERER_INFO_VENDOR,
            RENDERER_INFO_DEVICE
        };

        RendererBase() : batchedDrawState {}
        {
            size_t size                        = sizeof(uint16_t) * LOVE_UINT16_MAX;
            this->batchedDrawState.indexBuffer = new StreamBuffer(BUFFERUSAGE_INDEX, size);
        }

        size_t getVertexCount() const
        {
            return this->renderCtx.vertexCount;
        }

        Vertex* getVertices()
        {
            return this->data;
        }

        BatchedVertexData requestBatchDraw(const DrawCommand& command)
        {
            BatchedDrawState& state = this->batchedDrawState;

            bool shouldFlush  = false;
            bool shouldResize = false;

            // clang-format off
            if (command.primitiveMode != state.primitiveMode
                || command.format != state.format
                || ((command.indexMode != TRIANGLEINDEX_NONE) != (state.indexCount > 0))
                || command.texture != state.texture
                || command.shaderType != state.shader)
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

            {
                size_t stride   = getFormatStride(command.format);
                size_t dataSize = stride * totalVertices;

                if (state.vertices != nullptr && dataSize > state.verticesSize)
                    shouldFlush = true;

                if (dataSize > state.verticesSize)
                {
                    bufferSizes[0] = std::max<size_t>(dataSize, state.verticesSize * 1.1f);
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
                    bufferSizes[1] = std::max<size_t>(dataSize, state.indexBuffer->getSize() * 2);
                    shouldResize   = true;
                }
            }

            if (shouldFlush || shouldResize)
            {
                flushBatchedDraws();

                state.primitiveMode = command.primitiveMode;
                state.format        = command.format;
                state.texture       = command.texture;
                state.shader        = command.shaderType;
            }

            if (state.vertexCount == 0)
            {
                if (ShaderBase::isDefaultActive())
                    ShaderBase::attachDefault(state.shader);
            }

            if (shouldResize)
            {
                if (state.verticesSize < bufferSizes[0])
                {
                    linearFree(state.vertices);
                    state.vertices = (Vertex*)linearAlloc(bufferSizes[0]);
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
                {
                    const auto size = command.vertexCount * sizeof(Vertex);
                    state.vertexMap = StreamBuffer::MapInfo((uint8_t*)state.vertices, size);
                }

                data.stream = (Vertex*)state.vertexMap.data;
                state.vertexMap.data += newDataSize;
            }

            state.vertexCount += command.vertexCount;
            state.indexCount += requestedIndexCount;

            return data;
        }

        virtual void updateUniforms() = 0;

        void flushBatchedDraws()
        {
            BatchedDrawState& state = this->batchedDrawState;

            if ((state.vertexCount == 0 && state.indexCount == 0) || state.flushing)
                return;

            size_t usedSizes[2] = { 0, 0 };

            this->updateUniforms();

            if (state.format != CommonFormat::NONE)
                state.vertexMap = StreamBuffer::MapInfo();

            state.flushing = true;

            if (state.indexCount > 0)
            {
                DrawIndexedCommand command {};
                command.primitiveType     = state.primitiveMode;
                command.indexCount        = state.indexCount;
                command.indexType         = INDEX_UINT16;
                command.indexBufferOffset = state.indexBuffer->unmap();
                command.texture           = nullptr;

                this->draw(command);
                state.indexMap = StreamBuffer::MapInfo();
            }

            if (usedSizes[1] > 0)
                state.indexBuffer->markUsed(usedSizes[1]);

            state.vertexCount = 0;
            state.indexCount  = 0;
            state.flushing    = false;
        }

        static void flushBatchedDrawsGlobal()
        {
            RendererBase<T>::getInstance().flushBatchedDraws();
        }

      protected:
        struct ContextBase
        {
            bool dirtyProjection;

            CullMode cullMode;
            ColorChannelMask colorMask;
            BlendState blendState;
            StencilState stencilState;

            Rect scissor;
            Rect viewport;

            ShaderBase::StandardShader shader = ShaderBase::STANDARD_DEFAULT;
        };

        virtual void draw(const DrawIndexedCommand& command) = 0;

        bool initialized = false;
        bool inFrame     = false;

        BatchedDrawState batchedDrawState;
    };
} // namespace love
