#pragma once

#include "common/Exception.hpp"

#include "modules/graphics/StreamBuffer.tcc"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

namespace love
{
    struct BatchedDrawCommand
    {
        PrimitiveType primitiveMode           = PRIMITIVE_TRIANGLES;
        CommonFormat format                   = CommonFormat::NONE;
        TriangleIndexMode indexMode           = TRIANGLEINDEX_NONE;
        int vertexCount                       = 0;
        TextureBase* texture                  = nullptr;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;

        bool isFont        = false;
        bool pushTransform = true;
    };

    struct DrawIndexedCommand
    {
        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
        int indexCount              = 0;
        int instanceCount           = 1;

        VertexAttributesID attributesID;
        const BufferBindings* buffers;

        Resource* indexBuffer = nullptr;

        IndexDataType indexType  = INDEX_UINT16;
        size_t indexBufferOffset = 0;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;

        bool isFont = false;

        DrawIndexedCommand(VertexAttributesID attributesID, const BufferBindings* buffers,
                           Resource* indexBuffer) :
            attributesID(attributesID),
            buffers(buffers),
            indexBuffer(indexBuffer)
        {}
    };

    struct DrawCommand
    {
        DrawCommand(VertexAttributesID attributesID, const BufferBindings* buffers) :
            attributesID(attributesID),
            buffers(buffers)
        {}

        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;

        VertexAttributesID attributesID;
        const BufferBindings* buffers;

        int vertexStart   = 0;
        int vertexCount   = 0;
        int instanceCount = 1;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;
        bool isFont          = false;
    };

    struct BatchedVertexData
    {
        void* stream;
    };

    struct BatchedDrawState
    {
        StreamBufferBase* vertexBuffer = nullptr;
        StreamBufferBase* indexBuffer  = nullptr;

        PrimitiveType primitiveMode = PRIMITIVE_TRIANGLES;
        bool indexedDraw            = false;
        CommonFormat format         = CommonFormat::NONE;
        StrongRef<TextureBase> texture;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;

        int vertexCount = 0;
        int indexCount  = 0;

        int lastVertexCount = 0;
        int lastIndexCount  = 0;

        VertexAttributesID attributesIDs[(int)CommonFormat::COUNT] = {};

        StreamBufferBase::MapInfo vertexBufferMap = StreamBufferBase::MapInfo();
        StreamBufferBase::MapInfo indexBufferMap  = StreamBufferBase::MapInfo();

        bool flushing = false;

        bool isFont        = false;
        bool pushTransform = true;
    };

#if defined(__WIIU__)
    constexpr size_t INIT_VERTEX_BUFFER_SIZE = 1024 * (sizeof(Vertex) * 4);
#else
    constexpr size_t INIT_VERTEX_BUFFER_SIZE = 1024 * 1024 * 1;
#endif
    constexpr size_t INIT_INDEX_BUFFER_SIZE = sizeof(uint16_t) * LOVE_UINT16_MAX;

    StreamBufferBase* createStreamBuffer(BufferUsage usage, size_t size);
} // namespace love
