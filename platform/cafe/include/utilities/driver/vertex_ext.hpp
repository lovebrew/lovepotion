#pragma once

#include <common/color.hpp>

#include <utilities/driver/renderer/vertex.hpp>

#include <array>

#include <gx2/enum.h>

namespace love
{
    namespace vertex
    {
        struct Vertex
        {
            std::array<float, 3> position;
            std::array<float, 4> color;
            std::array<float, 2> texcoord;
        };

        static constexpr size_t VERTEX_SIZE = sizeof(Vertex);

        static std::array<float, 2> Normalize(const Vector2& in)
        {
            return { in.x, in.y };
        }
    } // namespace vertex
} // namespace love
