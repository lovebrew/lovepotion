#pragma once

#include "common/render/vertex.h"
#include "modules/graphics/graphics.h"

#include "objects/shader/shader.h"
#include "objects/texture/texture.h"

#include <memory>

namespace love
{
    struct BatchedDrawCommand
    {
        Texture* texture                     = nullptr;
        std::vector<uint32_t> textureHandles = {};

        Vertex::PrimitiveType primitiveMode = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat vertexFormat   = Vertex::CommonFormat::PRIMITIVE;

        /* shader enum to use */
        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;

        /* count of vertices */
        size_t count = 0;

        /* constructor for Primitives */
        BatchedDrawCommand(Vertex::PrimitiveType mode, size_t vertexCount,
                           Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT) :
            primitiveMode(mode),
            shaderType(shaderType),
            count(vertexCount)
        {}

        /* constructor for Textures */
        BatchedDrawCommand(Vertex::PrimitiveType mode, size_t vertexCount,
                           const std::vector<uint32_t>& handles,
                           Shader::StandardShader shaderType) :
            BatchedDrawCommand(mode, vertexCount, shaderType)
        {

            this->textureHandles = handles;
            this->vertexFormat   = Vertex::CommonFormat::TEXTURE;
        }
    };

    struct DrawCommand
    {
        Vertex::PrimitiveType primitiveMode  = Vertex::PRIMITIVE_TRIANGLES;
        std::vector<uint32_t> textureHandles = {};

        std::vector<Vertex::PrimitiveVertex> buffer = {};

        size_t vertexCount = 0;
        size_t size        = 0;
        size_t offset      = 0;
    };

    struct BatchedVertexData
    {
        std::vector<Vector2> positions;
        void* stream;
    };

    struct StreamDrawState
    {
        StrongReference<Texture> texture;
        std::vector<uint32_t> textureHandles = {};

        Shader::StandardShader shaderType  = Shader::STANDARD_DEFAULT;
        Vertex::PrimitiveType primitveMode = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat vertexFormat  = Vertex::CommonFormat::NONE;

        Vertex::CullMode cullMode = Vertex::CULL_NONE;

        std::vector<Vertex::PrimitiveVertex> vertices;

        /* count * sizeof(PrimitiveVertex) */
        size_t size = 0;

        /* count of vertices */
        size_t count = 0;
    };
} // namespace love
