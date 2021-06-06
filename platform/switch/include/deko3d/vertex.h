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

        Colorf color;
    };

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

    enum class TriangleIndexMode
    {
        NONE,
        STRIP,
        FAN,
        QUADS
    };

    static inline uint16_t normto16t(float in)
    {
        return uint16_t(in * 0xFFFF);
    }

    namespace attributes
    {
        /* Primitives */

        constexpr std::array<DkVtxBufferState, 1> PrimitiveBufferState = {
            DkVtxBufferState { sizeof(vertex::Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 2> PrimitiveAttribState = {
            DkVtxAttribState { 0, 0, offsetof(vertex::Vertex, position), DkVtxAttribSize_3x32,
                               DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::Vertex, color), DkVtxAttribSize_4x32,
                               DkVtxAttribType_Float, 0 }
        };

        /*  Textures */

        constexpr std::array<DkVtxBufferState, 1> TextureBufferState = {
            DkVtxBufferState { sizeof(vertex::Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 3> TextureAttribState = {
            DkVtxAttribState { 0, 0, offsetof(vertex::Vertex, position), DkVtxAttribSize_3x32,
                               DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::Vertex, color), DkVtxAttribSize_4x32,
                               DkVtxAttribType_Float, 0 },
            DkVtxAttribState { 0, 0, offsetof(vertex::Vertex, texcoord), DkVtxAttribSize_2x16,
                               DkVtxAttribType_Unorm, 0 }
        };
    } // namespace attributes

    [[nodiscard]] static inline std::unique_ptr<Vertex[]> GeneratePrimitiveFromVectors(
        std::span<Vector2> points, std::span<Colorf> colors)
    {
        Colorf color = colors[0];

        size_t pointCount = points.size();
        auto result       = std::make_unique<Vertex[]>(pointCount);

        size_t colorCount = colors.size();

        for (size_t index = 0; index < pointCount; index++)
        {
            const Vector2 point = points[index];

            if (index < colorCount)
                color = colors[index];

            Vertex append = { .position = { point.x, point.y, 0.0f },
                              .color    = { color.r, color.g, color.b, color.a },
                              .texcoord = { 0, 0 } };

            result[index] = append;
        }

        return result;
    }

    std::vector<Vertex> GenerateTextureFromVectors(const love::Vector2* points,
                                                   const love::Vector2* texcoord, size_t count,
                                                   Colorf color);

    std::vector<vertex::Vertex> GenerateTextureFromGlyphs(const vertex::GlyphVertex* verts,
                                                          size_t count);

    bool GetConstant(const char* in, CullMode& out);
    bool GetConstant(CullMode in, const char*& out);
    std::vector<std::string> GetConstants(CullMode);

    bool GetConstant(const char* in, Winding& out);
    bool GetConstant(Winding in, const char*& out);
    std::vector<std::string> GetConstants(Winding);
}; // namespace vertex
