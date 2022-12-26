#pragma once

#include <common/color.hpp>

#include <utilities/driver/renderer/vertex.hpp>

#include <array>

namespace love
{
    namespace vertex
    {
        struct Vertex
        {
            std::array<float, 3> position;
            std::array<float, 4> color;
            std::array<uint16_t, 2> texcoord;
        };

        static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

        static std::array<uint16_t, 2> Normalize(const Vector2& in)
        {
            return { normto16t(in.x), normto16t(in.y) };
        }
    } // namespace vertex
} // namespace love
