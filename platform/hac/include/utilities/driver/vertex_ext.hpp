#pragma once

#include <common/color.hpp>

#include <utilities/driver/renderer/vertex.hpp>

#include <deko3d.hpp>

#include <array>

namespace vertex
{
    struct Vertex
    {
        float position[3];
        float color[4];
        uint16_t texcoord[2];
    };

    struct GlyphVertex
    {
        float x, y;
        uint16_t s, t;

        Color color;
    };

    namespace attributes
    {
        // clang-format off
        /* Primitives */
        constexpr std::array<DkVtxBufferState, 1> PrimitiveBufferState = {
            DkVtxBufferState { sizeof(Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 2> PrimitiveAttribState = {
            DkVtxAttribState { 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 }
        };

        /* Textures*/
        constexpr std::array<DkVtxBufferState, 1> TextureBufferState = {
            DkVtxBufferState { sizeof(Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 3> TextureAttribState = {
            DkVtxAttribState { 0, 0, offsetof(Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(Vertex, texcoord), DkVtxAttribSize_2x16, DkVtxAttribType_Unorm, 0 }
        };
        // clang-format on
    } // namespace attributes
} // namespace vertex
