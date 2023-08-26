#pragma once

#include <common/console.hpp>
#include <common/vector.hpp>

#include <objects/shader/shader.tcc>
#include <objects/texture/texture.tcc>

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
                this->vertices  = std::make_unique<Vertex[]>(count);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        DrawCommand Clone()
        {
            DrawCommand clone(this->count);
            std::copy_n(this->Positions().get(), count, clone.Positions().get());
            std::copy_n(this->Vertices().get(), count, clone.Vertices().get());

            return clone;
        }

        const std::unique_ptr<Vector2[]>& Positions() const
        {
            return this->positions;
        }

        const std::unique_ptr<Vertex[]>& Vertices() const
        {
            return this->vertices;
        }

        /* primitive */
        void FillVertices(const Color& color)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { 0.0f, 0.0f }
                };
                // clang-format on
            }
        }

        /* primitive */
        void FillVertices(const Color* colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }
        }

        /* primitive */
        void FillVertices(std::span<Color> colors)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = colors[index].array(),
                    .texcoord = { 0, 0 }
                };
                // clang-format on
            }
        }

        /* texture */
        void FillVertices(const Color& color, const Vector2* textureCoords)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = color.array(),
                    .texcoord = { textureCoords[index].x, textureCoords[index].y }
                };
                // clang-format on
            }
        }

        /* font */
        void FillVertices(const Vertex* source)
        {
            for (size_t index = 0; index < this->count; index++)
            {
                // clang-format off
                this->vertices[index] =
                {
                    .position = { this->positions[index].x, this->positions[index].y, 0 },
                    .color    = source[index].color,
                    .texcoord = source[index].texcoord
                };
                // clang-format on
            }
        }

      public:
        std::unique_ptr<Vector2[]> positions;
        std::unique_ptr<Vertex[]> vertices;

        size_t count;
        size_t size;

        CommonFormat format;
        PrimitiveType type;
        Shader<>::StandardShader shader;

#if not defined(__3DS__)
        std::vector<Texture<Console::Which>*> handles;
#else
        std::vector<C3D_Tex*> handles;
#endif
    }; // namespace love
} // namespace love