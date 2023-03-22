#pragma once

#include <objects/shader_ext.hpp>
#include <objects/texture_ext.hpp>

#include <utilities/driver/buffer.hpp>
#include <utilities/driver/vertex_ext.hpp>

#include <memory>

namespace love
{
    struct DrawCommand
    {
      public:
        DrawCommand(int vertexCount) :
            handles {},
            count(vertexCount),
            stride(vertex::VERTEX_SIZE),
            format(vertex::CommonFormat::PRIMITIVE),
            primitveType(vertex::PRIMITIVE_TRIANGLES),
            shader(Shader<>::STANDARD_DEFAULT)
        {
            try
            {
                this->positions = std::make_unique<Vector2[]>(vertexCount);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        ~DrawCommand()
        {}

        const std::unique_ptr<Vector2[]>& Positions() const
        {
            return this->positions;
        }

        void FillVertices(const vertex::Vertex* data)
        {
            this->buffer  = std::make_shared<DrawBuffer>(this->count);
            auto vertices = this->buffer->Lock();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
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
            this->buffer  = std::make_shared<DrawBuffer>(this->count);
            auto vertices = this->buffer->Lock();

            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { textureCoords[index].x, textureCoords[index].y }
                };
                // clang-format on
            }
        }

        std::unique_ptr<Vector2[]> positions;
        std::vector<Texture<Console::CAFE>*> handles;

        std::shared_ptr<DrawBuffer> buffer;

        size_t count;
        size_t stride;

        vertex::CommonFormat format;
        vertex::PrimitiveType primitveType;
        Shader<>::StandardShader shader;
    };
} // namespace love
