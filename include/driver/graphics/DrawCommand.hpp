#pragma once

#include "common/Exception.hpp"

#include "driver/graphics/StreamBuffer.hpp"

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

        const VertexAttributes* attributes;
        const BufferBindings* buffers;

        Resource* indexBuffer = nullptr;

        IndexDataType indexType  = INDEX_UINT16;
        size_t indexBufferOffset = 0;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;

        bool isFont = false;

        DrawIndexedCommand(const VertexAttributes* attributes, const BufferBindings* buffers,
                           Resource* indexBuffer) :
            attributes(attributes),
            buffers(buffers),
            indexBuffer(indexBuffer)
        {}
    };

    struct DrawCommand
    {
        DrawCommand(const BufferBindings* buffers) : buffers(buffers)
        {}

        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;

        int vertexStart   = 0;
        int vertexCount   = 0;
        int instanceCount = 1;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;
        const BufferBindings* buffers;

        bool isFont = false;
    };

    struct BatchedVertexData
    {
        void* stream;
    };

    struct BatchedDrawState
    {
        StreamBuffer<Vertex>* vertexBuffer  = nullptr;
        StreamBuffer<uint16_t>* indexBuffer = nullptr;

        PrimitiveType primitiveMode = PRIMITIVE_TRIANGLES;
        CommonFormat format         = CommonFormat::NONE;
        StrongRef<TextureBase> texture;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;

        int vertexCount = 0;
        int indexCount  = 0;

        int lastVertexCount = 0;
        int lastIndexCount  = 0;

        MapInfo<Vertex> vertexBufferMap  = MapInfo<Vertex>();
        MapInfo<uint16_t> indexBufferMap = MapInfo<uint16_t>();

        bool flushing = false;

        bool isFont        = false;
        bool pushTransform = true;
    };

    constexpr size_t INIT_VERTEX_BUFFER_SIZE = sizeof(Vertex) * 4096 * 1;
    constexpr size_t INIT_INDEX_BUFFER_SIZE  = sizeof(uint16_t) * LOVE_UINT16_MAX * 1;

    inline StreamBuffer<Vertex>* newVertexBuffer(size_t size)
    {
        return new StreamBuffer<Vertex>(BUFFERUSAGE_VERTEX, size);
    }

    inline StreamBuffer<uint16_t>* newIndexBuffer(size_t size)
    {
        return new StreamBuffer<uint16_t>(BUFFERUSAGE_INDEX, size);
    }
} // namespace love
