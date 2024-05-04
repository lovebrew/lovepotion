#pragma once

#include "common/Exception.hpp"

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

#include "driver/graphics/StreamBuffer.hpp"

namespace love
{
    struct BatchedDrawCommand
    {
        PrimitiveType primitiveMode           = PRIMITIVE_TRIANGLES;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;
        TriangleIndexMode indexMode           = TRIANGLEINDEX_NONE;
        CommonFormat format                   = CommonFormat::NONE;

        TextureBase* texture = nullptr;
        int vertexCount      = 0;
    };

    struct DrawIndexedCommand
    {
        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;
        int indexCount              = 0;
        int instanceCount           = 1;

        IndexDataType indexType  = INDEX_UINT16;
        size_t indexBufferOffset = 0;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;
    };

    struct DrawCommand
    {
        PrimitiveType primitiveType = PRIMITIVE_TRIANGLES;

        int vertexStart   = 0;
        int vertexCount   = 0;
        int instanceCount = 1;

        TextureBase* texture = nullptr;
        CullMode cullMode    = CULL_NONE;
    };

    struct BatchedVertexData
    {
        void* stream;
    };

    struct BatchedDrawState
    {
        StreamBuffer* vertexBuffer = nullptr;
        StreamBuffer* indexBuffer  = nullptr;

        PrimitiveType primitiveMode = PRIMITIVE_TRIANGLES;
        CommonFormat format         = CommonFormat::NONE;
        StrongRef<TextureBase> texture;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;

        int vertexCount = 0;
        int indexCount  = 0;

        StreamBuffer::MapInfo vertexMap = StreamBuffer::MapInfo();
        StreamBuffer::MapInfo indexMap  = StreamBuffer::MapInfo();

        bool flushing = false;
    };

    static constexpr auto INIT_INDEX_BUFFER_SIZE  = sizeof(uint16_t) * LOVE_UINT16_MAX;
    static constexpr auto INIT_VERTEX_BUFFER_SIZE = 64 * 1024 * 1;
} // namespace love
