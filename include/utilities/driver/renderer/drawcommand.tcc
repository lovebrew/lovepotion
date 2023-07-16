#pragma once

#include <common/console.hpp>
#include <common/vector.hpp>

#include <objects/shader/shader.tcc>

#include <utilities/driver/renderer/drawbuffer.tcc>
#include <utilities/driver/renderer/vertex.hpp>

#include <memory>

namespace love
{
    using namespace vertex;

    template<Console::Platform T = Console::ALL>
    struct DrawCommand
    {
      public:
        DrawCommand()
        {}

        DrawCommand(size_t count, PrimitiveType type = PRIMITIVE_TRIANGLES,
                    Shader<>::StandardShader shader = Shader<>::STANDARD_DEFAULT) :
            positions {},
            count(count),
            size(count * VERTEX_SIZE),
            format(CommonFormat::PRIMITIVE),
            type(type),
            shader(shader)
        {
            if (count == 0)
                throw love::Exception("Vertex count cannot be zero.");

            try
            {
                this->positions = std::make_unique<Vector2[]>(count);
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

        void FillVertices(Vertex* vertices, const Color& color)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { 0.0f, 0.0f }
                };
                // clang-format on
            }
        }

        void FillVertices(Vertex* vertices, const Color* colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }
        }

        void FillVertices(Vertex* vertices, std::span<Color> colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }
        }

      public:
        std::unique_ptr<Vector2[]> positions;
        size_t count;
        size_t size;

        CommonFormat format;
        PrimitiveType type;
        Shader<>::StandardShader shader;

        std::shared_ptr<DrawBuffer<Console::Which>> buffer;
    }; // namespace love
} // namespace love