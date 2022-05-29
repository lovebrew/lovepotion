#pragma once

#include "common/colors.h"

#include <stdint.h>
#include <vector>

namespace love
{
    namespace Vertex
    {
        struct PrimitiveVertex
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

        enum TriangleIndexMode
        {
            TRIANGLE_NONE,
            TRIANGLE_STRIP,
            TRIANGLE_FAN,
            TRIANGLE_QUADS
        };

        static inline uint16_t normto16t(float in)
        {
            return uint16_t(in * 0xFFFF);
        }

        static bool GetConstant(const char* in, Winding& out);
        static bool GetConstant(Winding in, const char*& out);
        static std::vector<const char*> GetConstants(Winding);

        static bool GetConstant(const char* in, CullMode& out);
        static bool GetConstant(CullMode in, const char*& out);
        static std::vector<const char*> GetConstants(CullMode);

        static bool GetConstant(const char* in, TriangleIndexMode& out);
        static bool GetConstant(TriangleIndexMode in, const char*& out);
        static std::vector<const char*> GetConstants(TriangleIndexMode);
    } // namespace Vertex
} // namespace love
