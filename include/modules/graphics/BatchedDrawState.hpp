#pragma once

#include "modules/graphics/Shader.tcc"
#include "modules/graphics/Texture.tcc"
#include "modules/graphics/vertex.hpp"

#include "modules/graphics/StreamBuffer.hpp"

namespace love
{
    struct BatchedDrawState
    {
        StreamBufferBase* vb[2];
        StreamBufferBase* indexBuffer = nullptr;

        PrimitiveType primitiveMode = PRIMITIVE_TRIANGLES;
        CommonFormat formats[2]     = { CommonFormat::NONE, CommonFormat::NONE };
        StrongRef<TextureBase> texture;
        ShaderBase::StandardShader shaderType = ShaderBase::STANDARD_DEFAULT;

        StreamBufferBase::MapInfo vbMap[2] = { StreamBufferBase::MapInfo(), StreamBufferBase::MapInfo() };
        StreamBufferBase::MapInfo indexBufferMap = StreamBufferBase::MapInfo();

        int vertexCount;
        int indexCount;

        bool flushing = false;
    };
} // namespace love
