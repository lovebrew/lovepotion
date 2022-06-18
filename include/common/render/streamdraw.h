#pragma once

#include "common/render/vertex.h"
#include "modules/graphics/graphics.h"

#include "objects/shader/shader.h"
#include "objects/texture/texture.h"

#include <memory>

namespace love
{
    struct DrawCommand
    {
        Texture* texture                     = nullptr;
        Vertex::PrimitiveType primitiveMode  = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat vertexFormat    = Vertex::CommonFormat::NONE;
        std::vector<uint32_t> textureHandles = {};

        /* vertices to upload data to */
        std::unique_ptr<Vertex::PrimitiveVertex[]> vertices = nullptr;

        /* positional vectors */
        std::unique_ptr<Vector2[]> positions = nullptr;

        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;

        /* count of how many vertices */
        size_t count = 0;

        /* size of vertices, auto-calculated */
        size_t size = 0;

        /* constructor for Primitives */
        DrawCommand(Vertex::PrimitiveType mode, size_t count,
                    Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT) :
            primitiveMode(mode),
            shaderType(shaderType),
            count(count),
            size(count * Vertex::PRIM_VERTEX_SIZE)
        {
            this->vertices  = std::make_unique<Vertex::PrimitiveVertex[]>(count);
            this->positions = std::make_unique<Vector2[]>(count);
        }

        /* constructor for Textures */
        DrawCommand(Vertex::PrimitiveType type, size_t count, const std::vector<uint32_t>& handles,
                    Shader::StandardShader shaderType) :
            DrawCommand(type, count, shaderType)
        {

            this->textureHandles = handles;
            this->vertexFormat   = Vertex::CommonFormat::TEXTURE;
        }

        Vertex::PrimitiveVertex* GetVertices()
        {
            return this->vertices.get();
        }

        Vector2* GetPositions()
        {
            return this->positions.get();
        }
    };

    struct StreamDrawState
    {
        StrongReference<Texture> texture;
        std::vector<uint32_t> textureHandles = {};
        Shader::StandardShader shaderType    = Shader::STANDARD_DEFAULT;
        Vertex::PrimitiveType primitveMode   = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat vertexFormat    = Vertex::CommonFormat::NONE;

        std::vector<Vertex::PrimitiveVertex> vertices;

        size_t size  = 0;
        size_t count = 0;
    };
} // namespace love
