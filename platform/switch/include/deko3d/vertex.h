#pragma once

#include "deko3d/CMemPool.h"
#include "deko3d/common.h"

#include "common/vector.h"
#include "common/colors.h"

#include <array>
#include <memory>

namespace vertex
{
    struct Vertex
    {
        float position[3];
        float color[4];
        float texcoord[2];
    };

    struct GlyphVertex
    {
        float x, y;
        float s, t;

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

    namespace attributes
    {
        /* Primitives */

        constexpr std::array<DkVtxBufferState, 1> PrimitiveBufferState =
        {
            DkVtxBufferState{ sizeof(vertex::Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 2> PrimitiveAttribState =
        {
            DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 }
        };

        /*  Textures */

        constexpr std::array<DkVtxBufferState, 1> TextureBufferState =
        {
            DkVtxBufferState{ sizeof(vertex::Vertex), 0 },
        };

        constexpr std::array<DkVtxAttribState, 3> TextureAttribState =
        {
            DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, position), DkVtxAttribSize_3x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, color),    DkVtxAttribSize_4x32, DkVtxAttribType_Float, 0 },
            DkVtxAttribState{ 0, 0, offsetof(vertex::Vertex, texcoord), DkVtxAttribSize_2x32, DkVtxAttribType_Float, 0 }
        };
    }

    std::vector<Vertex> GeneratePrimitiveFromVectors(const love::Vector2 * points, size_t count,
                                                     const Colorf * colors, size_t colorCount);

    std::vector<Vertex> GenerateTextureFromVectors(const love::Vector2 * points, const love::Vector2 * texcoord,
                                                   size_t count, Colorf color);

    std::vector<vertex::Vertex> GenerateTextureFromGlyphs(const GlyphVertex * gVerts, size_t count);

    void DebugVertex(const vertex::Vertex & vertex);

    void DebugVertex(const vertex::GlyphVertex & vertex);

    bool GetConstant(const char * in, CullMode & out);
    bool GetConstant(CullMode in, const char *& out);
    std::vector<std::string> GetConstants(CullMode);

    bool GetConstant(const char * in, Winding & out);
    bool GetConstant(Winding in, const char *& out);
    std::vector<std::string> GetConstants(Winding);
};