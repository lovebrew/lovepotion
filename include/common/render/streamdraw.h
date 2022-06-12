#pragma once

#include "common/render/vertex.h"
#include "modules/graphics/graphics.h"

#include "objects/shader/shader.h"
#include "objects/texture/texture.h"

namespace love
{
    struct StreamDrawCommand
    {
        Vertex::PrimitiveType primitveMode   = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat formats[2]      = { Vertex::CommonFormat::NONE };
        Vertex::TriangleIndexMode indexMode  = Vertex::TRIANGLE_NONE;
        std::vector<uint32_t> textureHandles = {};

        int vertices                      = 0;
        Texture* texture                  = nullptr;
        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;
    };

    /* { position, quads } */
    struct StreamVertexData
    {
        void* stream[2];
    };

    struct StreamBufferState
    {
        Vertex::PrimitiveType primitiveMode = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat formats[2]     = { Vertex::CommonFormat::NONE };

        uint8_t* buffers[2];
        size_t bufferSizes[2] = {0, 0};

        StrongReference<Texture> texture;
        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;

        std::vector<uint32_t> textureHandles = {};

        int vertices = 0;
        int indecies = 0;
    };
} // namespace love
