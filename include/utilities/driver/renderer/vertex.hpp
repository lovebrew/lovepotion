#pragma once

#include <common/vector.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

namespace love
{
    namespace Vertex
    {
        enum CullMode
        {
            CULL_NONE,
            CULL_BACK,
            CULL_FRONT,
            CULL_MAX_ENUM
        };

        enum Winding
        {
            WINDING_CW,
            WINDING_CCW,
            WINDING_MAX_ENUM
        };

        enum TriangleIndexMode
        {
            TRIANGLE_NONE,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            TRIANGLE_QUADS
        };

        enum PrimitiveType
        {
            PRIMITIVE_TRIANGLES,
            PRIMITIVE_TRIANGLE_STRIP,
            PRIMITIVE_TRIANGLE_FAN,
            PRIMITIVE_QUADS,
            PRIMITIVE_POINTS,
            PRIMITIVE_MAX_ENUM
        };

        /*
        ** Position, Color
        ** Position, TexCoords, Color
        */
        enum class CommonFormat
        {
            NONE,
            PRIMITIVE,
            TEXTURE
        };

        static inline uint16_t normto16t(float in)
        {
            return uint16_t(in * 0xFFFF);
        }

        // clang-format off
        static constexpr BidirectionalMap cullModes = {
            "none",  Vertex::CULL_NONE,
            "back",  Vertex::CULL_BACK,
            "front", Vertex::CULL_FRONT
        };

        static constexpr BidirectionalMap windingModes = {
            "cw",  Vertex::WINDING_CW,
            "ccw", Vertex::WINDING_CCW
        };

        static constexpr BidirectionalMap triangleModes = {
            "none",  Vertex::TRIANGLE_NONE,
            "strip", Vertex::TRIANGLE_STRIP,
            "fan",   Vertex::TRIANGLE_FAN,
            "quads", Vertex::TRIANGLE_QUADS
        };
        // clang-format on
    } // namespace Vertex
} // namespace love
