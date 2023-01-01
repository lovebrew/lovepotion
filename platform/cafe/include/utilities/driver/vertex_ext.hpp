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

        static constexpr size_t POSITION_OFFSET = offsetof(Vertex, position);
        static constexpr size_t COLOR_OFFSET    = offsetof(Vertex, color);
        static constexpr size_t TEXCOORD_OFFSET = offsetof(Vertex, texcoord);
    } // namespace vertex
} // namespace love
