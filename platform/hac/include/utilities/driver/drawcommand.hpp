#pragma once

#include <common/vector.hpp>

#include <utilities/driver/vertex_ext.hpp>

#include <objects/shader_ext.hpp>

#include <memory>

namespace love
{
    struct DrawCommand
    {
      public:
        DrawCommand(int vertexCount, vertex::PrimitiveType type = vertex::PRIMITIVE_TRIANGLES,
                    Shader<>::StandardShader shader = Shader<>::STANDARD_DEFAULT) :
            positions {},
            vertices {},
            count(vertexCount),
            size(vertexCount * vertex::VERTEX_SIZE),
            handles {},
            format(vertex::CommonFormat::PRIMITIVE),
            primitveType(type),
            shader(shader)
        {
            try
            {
                this->positions = std::make_unique<Vector2[]>(vertexCount);
                this->vertices  = std::make_unique<vertex::Vertex[]>(vertexCount);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        const std::unique_ptr<Vector2[]>& Positions() const
        {
            return this->positions;
        }

        const std::unique_ptr<vertex::Vertex[]>& Vertices() const
        {
            return this->vertices;
        }

        void FillVertices(const Color& color)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array()
                };
                // clang-format on
            }
        }

        void FillVertices(std::span<Color> colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array()
                };
                // clang-format on
            }
        }

        void FillVertices(Color* colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array()
                };
                // clang-format on
            }
        }

        void FillVertices(const vertex::Vertex* data)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = data[index].color,
                    .texcoord = data[index].texcoord
                };
                // clang-format on
            }
        }

        void FillVertices(const Color& color, const Vector2* textureCoords)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = vertex::Normalize(textureCoords[index])
                };
                // clang-format on
            }
        }

        std::unique_ptr<Vector2[]> positions;
        std::unique_ptr<vertex::Vertex[]> vertices;

        size_t count;
        size_t size;

        std::vector<DkResHandle> handles;

        vertex::CommonFormat format;
        vertex::PrimitiveType primitveType;
        Shader<>::StandardShader shader;
    };
} // namespace love
