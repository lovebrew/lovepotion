#pragma once

#include "common/render/vertex.h"
#include "modules/graphics/graphics.h"

namespace love
{
    struct StreamDrawCommand
    {
        Vertex::PrimitiveType primitveMode  = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat formats[2]     = { Vertex::CommonFormat::NONE };
        Vertex::TriangleIndexMode indexMode = Vertex::TRIANGLE_NONE;

        int vertices                      = 0;
        Texture* texture                  = nullptr;
        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;
    };

    /* { position, quads } */
    struct StreamVertexData
    {
        void* stream;
    };

    struct StreamBufferState
    {
        Vertex::PrimitiveType primitiveMode = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat formats[2]     = { Vertex::CommonFormat::NONE };
        void* vertexData                    = nullptr;

        StrongReference<Texture> texture;
        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;

        std::vector<const uint32_t> textureHandles = {};

        int vertices = 0;
        int indecies = 0;
    }
} // namespace love
