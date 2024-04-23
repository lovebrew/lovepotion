#pragma once

#include "common/Color.hpp"
#include "common/Map.hpp"
#include "common/int.hpp"

#include "common/Vector.hpp"

namespace love
{
    enum CullMode
    {
        CULL_NONE,
        CULL_FRONT,
        CULL_BACK,
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
        TRIANGLEINDEX_NONE,
        TRIANGLEINDEX_STRIP,
        TRIANGLEINDEX_FAN,
        TRIANGLEINDEX_QUADS
    };

    enum CommonFormat
    {
        NONE,         //< No format specified.
        XYf,          //< 2D vertex with floating-point position.
        XYZf,         //< 3D vertex with floating-point position.
        RGBAf,        //< 4-component color.
        STf_RGBAf,    //< 2D texture coordinate and 4-component color.
        XYf_STf,      //< 2D vertex with floating-point position and 2D texture coordinate.
        XYf_STf_RGBAf //< 2D vertex with floating-point position, 2D texture coordinate and
                      // 4-component color.
    };

    enum PrimitiveType
    {
        PRIMITIVE_TRIANGLES,
        PRIMITIVE_TRIANGLE_STRIP,
        PRIMITIVE_TRIANGLE_FAN,
        PRIMITIVE_POINTS,
        PRIMITIVE_MAX_ENUM
    };

    struct Vertex
    {
        Vector3 position;
        Vector2 texcoord;
        Color color;
    };

    inline CommonFormat getSinglePositionFormat(bool is2D)
    {
        return is2D ? XYf : XYZf;
    }

    // clang-format off
    STRINGMAP_DECLARE(PrimitiveTypes, PrimitiveType,
        { "triangles", PRIMITIVE_TRIANGLES      },
        { "strip",     PRIMITIVE_TRIANGLE_STRIP },
        { "fan",       PRIMITIVE_TRIANGLE_FAN   },
        { "points",    PRIMITIVE_POINTS         }
    );

    STRINGMAP_DECLARE(CullModes, CullMode,
        { "none",  CULL_NONE  },
        { "front", CULL_FRONT },
        { "back",  CULL_BACK  }
    );

    STRINGMAP_DECLARE(WindingModes, Winding,
        { "cw",  WINDING_CW  },
        { "ccw", WINDING_CCW }
    );
    // clang-format on
} // namespace love
