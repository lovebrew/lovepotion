#include "common/render/vertex.h"

#include "common/bidirectionalmap.h"

namespace love::Vertex
{
    // clang-format off
    constexpr auto cullModes = BidirectionalMap<>::Create(
        "none",  CULL_NONE,
        "back",  CULL_BACK,
        "front", CULL_FRONT
    );

    constexpr auto windModes = BidirectionalMap<>::Create(
        "cw",  WINDING_CW,
        "ccw", WINDING_CCW
    );

    constexpr auto triangleModes = BidirectionalMap<>::Create(
        "none",  TRIANGLE_NONE,
        "strip", TRIANGLE_STRIP,
        "fan",   TRIANGLE_FAN,
        "quads", TRIANGLE_QUADS
    );
    // clang-format on

    // CullMode
    bool GetConstant(const char* in, CullMode& out)
    {
        return cullModes.Find(in, out);
    }

    bool GetConstant(CullMode in, const char*& out)
    {
        return cullModes.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(CullMode)
    {
        return cullModes.GetNames();
    }

    // Winding
    bool GetConstant(const char* in, Winding& out)
    {
        return windModes.Find(in, out);
    }

    bool GetConstant(Winding in, const char*& out)
    {
        return windModes.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(Winding)
    {
        return windModes.GetNames();
    }

    // TriangleIndex
    bool GetConstant(const char* in, TriangleIndexMode& out)
    {
        return triangleModes.Find(in, out);
    }

    bool GetConstant(TriangleIndexMode in, const char*& out)
    {
        return triangleModes.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(TriangleIndexMode)
    {
        return triangleModes.GetNames();
    }
} // namespace love::Vertex
