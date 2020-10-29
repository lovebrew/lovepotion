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

    std::vector<Vertex> GenerateFromVectors(const love::Vector2 * points, size_t count, const Colorf * colors, size_t colorCount);

    bool GetConstant(const char * in, CullMode & out);
    bool GetConstant(CullMode in, const char *& out);
    std::vector<std::string> GetConstants(CullMode);

    bool GetConstant(const char * in, Winding & out);
    bool GetConstant(Winding in, const char *& out);
    std::vector<std::string> GetConstants(Winding);
};