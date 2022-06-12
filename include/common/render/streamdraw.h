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
        Vertex::PrimitiveType primitiveMode  = Vertex::PRIMITIVE_TRIANGLES;
        Vertex::CommonFormat format          = Vertex::CommonFormat::NONE;
        std::vector<uint32_t> textureHandles = {};

        std::unique_ptr<Vertex::PrimitiveVertex[]> vertices = nullptr;
        std::unique_ptr<Vector2[]> positions                = nullptr;

        Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT;

        size_t count;
        size_t size;

        /* constructor for Primitives */
        DrawCommand(Vertex::PrimitiveType mode, size_t count,
                    Shader::StandardShader shaderType = Shader::STANDARD_DEFAULT) :
            primitiveMode(mode),
            shaderType(shaderType),
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
            this->format         = Vertex::CommonFormat::TEXTURE;
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
} // namespace love
