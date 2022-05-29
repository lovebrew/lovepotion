#pragma once

#include "deko3d/CMemPool.h"
#include "deko3d/common.h"

#include "common/colors.h"
#include "common/vector.h"

#include <array>
#include <memory>
#include <span>

using namespace love;

namespace vertex
{
    namespace attributes
    {
        /* Primitives */

        constexpr std::array<DkVtxBufferState, 1> PrimitiveBufferState = {
            DkVtxBufferState { sizeof(vertex::PrimitiveVertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 2> PrimitiveAttribState = {
            DkVtxAttribState { 0, 0, offsetof(vertex::PrimitiveVertex, position),
                               DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::PrimitiveVertex, color), DkVtxAttribSize_4x32,
                               DkVtxAttribType_Float, 0 }
        };

        /*  Textures */

        constexpr std::array<DkVtxBufferState, 1> TextureBufferState = {
            DkVtxBufferState { sizeof(vertex::PrimitiveVertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 3> TextureAttribState = {
            DkVtxAttribState { 0, 0, offsetof(vertex::PrimitiveVertex, position),
                               DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::PrimitiveVertex, color), DkVtxAttribSize_4x32,
                               DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::PrimitiveVertex, texcoord),
                               DkVtxAttribSize_2x16, DkVtxAttribType_Unorm, 0 }
        };
    } // namespace attributes

    [[nodiscard]] static inline std::unique_ptr<PrimitiveVertex[]> GeneratePrimitiveFromVectors(
        std::span<Vector2> points, std::span<Colorf> colors)
    {
        Colorf color = colors[0];

        size_t pointCount = points.size();
        auto result       = std::make_unique<PrimitiveVertex[]>(pointCount);

        size_t colorCount = colors.size();

        for (size_t index = 0; index < pointCount; index++)
        {
            const Vector2 point = points[index];

            if (index < colorCount)
                color = colors[index];

            PrimitiveVertex append = { .position = { point.x, point.y, 0.0f },
                                       .color    = { color.r, color.g, color.b, color.a },
                                       .texcoord = { 0, 0 } };

            result[index] = append;
        }

        return result;
    }

    std::vector<PrimitiveVertex> GenerateTextureFromVectors(const love::Vector2* points,
                                                            const love::Vector2* texcoord,
                                                            size_t count, Colorf color);

    std::vector<vertex::PrimitiveVertex> GenerateTextureFromGlyphs(const vertex::GlyphVertex* verts,
                                                                   size_t count);
}; // namespace vertex
