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

#if defined(__3DS__)
    #include <citro3d.h>
    using Handle = C3D_Tex;
#else
    using Handle = Texture<Console::Which>;
#endif
    struct DrawCommand
    {
      public:
        DrawCommand()
        {}

        std::span<vertex::Vertex> AllocateVertices(size_t count);

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
                this->positions = std::vector<Vector2>(count);
                this->vertices  = AllocateVertices(count);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        DrawCommand(size_t count, Shader<>::StandardShader shader, CommonFormat format) :
            count(count),
            size(count * VERTEX_SIZE),
            format(format),
            shader(shader)
        {
            if (count == 0)
                throw love::Exception("Vertex count cannot be zero.");

            try
            {
                this->vertices = AllocateVertices(count);
            }
            catch (std::bad_alloc&)
            {
                throw love::Exception("Out of memory.");
            }
        }

        DrawCommand(std::span<vertex::Vertex> vertices, PrimitiveType type,
                    Shader<>::StandardShader shader, CommonFormat format) :
            vertices(vertices),
            count(vertices.size()),
            size(count * VERTEX_SIZE),
            format(format),
            type(type),
            shader(shader)
        {
            if (this->count == 0)
                throw love::Exception("Vertex count cannot be zero.");
        }

        std::optional<std::vector<Vector2>>& Positions()
        {
            return this->positions;
        }

        const std::optional<std::vector<Vector2>>& Positions() const
        {
            return this->positions;
        }

        std::span<const Vector2> GetPositions() const
        {
            return this->positions ? *this->positions : std::span<const Vector2> {};
        }

        std::span<const Vertex> Vertices() const
        {
            return std::span<const Vertex>(this->vertices.data(), this->vertices.size());
        }

        std::span<Vertex> Vertices()
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
                    .position = { (*this->positions)[index].x, (*this->positions)[index].y, 0 },
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
                    .position = { (*this->positions)[index].x, (*this->positions)[index].y, 0 },
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
                    .position = { (*this->positions)[index].x, (*this->positions)[index].y, 0 },
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
                    .position = { (*this->positions)[index].x, (*this->positions)[index].y, 0 },
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
                    .position = { (*this->positions)[index].x, (*this->positions)[index].y, 0 },
                    .color    = source[index].color,
                    .texcoord = source[index].texcoord
                };
                // clang-format on
            }
        }

      public:
        std::optional<std::vector<Vector2>> positions;
        std::span<Vertex> vertices;

        size_t count;
        size_t size;

        CommonFormat format;
        PrimitiveType type;
        Shader<>::StandardShader shader;
        std::vector<Handle*> handles;
        CullMode cullMode;
    }; // namespace love
} // namespace love
