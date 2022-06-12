#include "common/render/vertex.h"
#include "common/bidirectionalmap.h"

using namespace love;

// clang-format off
constexpr auto cullModes = BidirectionalMap<>::Create(
    "none",  Vertex::CULL_NONE,
    "back",  Vertex::CULL_BACK,
    "front", Vertex::CULL_FRONT
);

constexpr auto windModes = BidirectionalMap<>::Create(
    "cw",  Vertex::WINDING_CW,
    "ccw", Vertex::WINDING_CCW
);

constexpr auto triangleModes = BidirectionalMap<>::Create(
    "none",  Vertex::TRIANGLE_NONE,
    "strip", Vertex::TRIANGLE_STRIP,
    "fan",   Vertex::TRIANGLE_FAN,
    "quads", Vertex::TRIANGLE_QUADS
);
// clang-format on

namespace love::Vertex
{
    size_t GetFormatStride(CommonFormat format)
    {
        switch (format)
        {
            case CommonFormat::NONE:
                return 0;
            case CommonFormat::XYf:
                return sizeof(float) * 2;
            case CommonFormat::XYZf:
                return sizeof(float) * 3;
            case CommonFormat::RGBAub:
                return sizeof(uint8_t) * 4;
            case CommonFormat::STf_RGBAub:
                return PRIM_VERTEX_SIZE;
        }
    }

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
