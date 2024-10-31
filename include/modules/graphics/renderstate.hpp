#pragma once

#include "common/int.hpp"
#include "common/math.hpp"
#include "modules/graphics/vertex.hpp"

#include "common/Map.hpp"

namespace love
{
    class Shader;

    enum BlendMode
    {
        BLEND_ALPHA,
        BLEND_ADD,
        BLEND_SUBTRACT,
        BLEND_MULTIPLY,
        BLEND_LIGHTEN,
        BLEND_DARKEN,
        BLEND_SCREEN,
        BLEND_REPLACE,
        BLEND_NONE,
        BLEND_CUSTOM,
        BLEND_MAX_ENUM
    };

    enum BlendAlpha
    {
        BLENDALPHA_MULTIPLY,
        BLENDALPHA_PREMULTIPLIED,
        BLEND_ALPHA_MAX_ENUM
    };

    enum BlendFactor
    {
        BLENDFACTOR_ZERO,
        BLENDFACTOR_ONE,
        BLENDFACTOR_SRC_COLOR,
        BLENDFACTOR_ONE_MINUS_SRC_COLOR,
        BLENDFACTOR_SRC_ALPHA,
        BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        BLENDFACTOR_DST_COLOR,
        BLENDFACTOR_ONE_MINUS_DST_COLOR,
        BLENDFACTOR_DST_ALPHA,
        BLENDFACTOR_ONE_MINUS_DST_ALPHA,
        BLENDFACTOR_SRC_ALPHA_SATURATED,
        BLENDFACTOR_MAX_ENUM
    };

    enum BlendOperation
    {
        BLENDOP_ADD,
        BLENDOP_SUBTRACT,
        BLENDOP_REVERSE_SUBTRACT,
        BLENDOP_MIN,
        BLENDOP_MAX,
        BLENDOP_MAX_ENUM
    };

    enum StencilMode // High level wrappers.
    {
        STENCIL_MODE_OFF,
        STENCIL_MODE_DRAW,
        STENCIL_MODE_TEST,
        STENCIL_MODE_CUSTOM,
        STENCIL_MODE_MAX_ENUM
    };

    enum StencilAction
    {
        STENCIL_KEEP,
        STENCIL_ZERO,
        STENCIL_REPLACE,
        STENCIL_INCREMENT,
        STENCIL_DECREMENT,
        STENCIL_INCREMENT_WRAP,
        STENCIL_DECREMENT_WRAP,
        STENCIL_INVERT,
        STENCIL_MAX_ENUM
    };

    enum CompareMode
    {
        COMPARE_LESS,
        COMPARE_LEQUAL,
        COMPARE_EQUAL,
        COMPARE_GEQUAL,
        COMPARE_GREATER,
        COMPARE_NOTEQUAL,
        COMPARE_ALWAYS,
        COMPARE_NEVER,
        COMPARE_MAX_ENUM
    };

    struct BlendState
    {
        BlendOperation operationRGB = BLENDOP_ADD;
        BlendOperation operationA   = BLENDOP_ADD;
        BlendFactor srcFactorRGB    = BLENDFACTOR_ONE;
        BlendFactor srcFactorA      = BLENDFACTOR_ONE;
        BlendFactor dstFactorRGB    = BLENDFACTOR_ZERO;
        BlendFactor dstFactorA      = BLENDFACTOR_ZERO;
        bool enable                 = false;

        BlendState()
        {}

        BlendState(BlendOperation opRGB, BlendOperation opA, BlendFactor srcRGB, BlendFactor srcA,
                   BlendFactor dstRGB, BlendFactor dstA) :
            operationRGB(opRGB),
            operationA(opA),
            srcFactorRGB(srcRGB),
            srcFactorA(srcA),
            dstFactorRGB(dstRGB),
            dstFactorA(dstA),
            enable(true)
        {}

        bool operator==(const BlendState& other) const
        {
            return enable == other.enable && operationRGB == other.operationRGB &&
                   operationA == other.operationA && srcFactorRGB == other.srcFactorRGB &&
                   srcFactorA == other.srcFactorA && dstFactorRGB == other.dstFactorRGB &&
                   dstFactorA == other.dstFactorA;
        }
    };

    struct DepthState
    {
        CompareMode compare = COMPARE_ALWAYS;
        bool write          = false;

        bool operator==(const DepthState& d) const
        {
            return compare == d.compare && write == d.write;
        }
    };

    struct StencilState
    {
        CompareMode compare  = COMPARE_ALWAYS;
        StencilAction action = STENCIL_KEEP;
        int value            = 0;
        uint32_t readMask    = 0xFFFFFFFF;
        uint32_t writeMask   = 0xFFFFFFFF;

        bool operator==(const StencilState& s) const
        {
            return compare == s.compare && action == s.action && value == s.value && readMask == s.readMask &&
                   writeMask == s.writeMask;
        }
    };

    struct ColorChannelMask
    {
        bool r : 1, g : 1, b : 1, a : 1;

        ColorChannelMask() : r(true), g(true), b(true), a(true)
        {}

        ColorChannelMask(bool _r, bool _g, bool _b, bool _a) : r(_r), g(_g), b(_b), a(_a)
        {}

        bool operator==(const ColorChannelMask& m) const
        {
            return r == m.r && g == m.g && b == m.b && a == m.a;
        }

        bool operator!=(const ColorChannelMask& m) const
        {
            return !(operator==(m));
        }

        uint8_t get() const
        {
            return r | (g << 1) | (b << 2) | (a << 3);
        }
    };

    struct ScissorState
    {
        Rect rectangle = { 0, 0, 0, 0 };
        bool enable    = false;
    };

    BlendState computeBlendState(BlendMode mode, BlendAlpha alphamode);
    BlendMode computeBlendMode(BlendState s, BlendAlpha& alphamode);
    bool isAlphaMultiplyBlendSupported(BlendMode mode);

    StencilState computeStencilState(StencilMode mode, int value);
    StencilMode computeStencilMode(const StencilState& s);

    // clang-format off
    STRINGMAP_DECLARE(BlendModes, BlendMode,
        { "alpha",    BLEND_ALPHA    },
        { "add",      BLEND_ADD      },
        { "subtract", BLEND_SUBTRACT },
        { "multiply", BLEND_MULTIPLY },
        { "lighten",  BLEND_LIGHTEN  },
        { "darken",   BLEND_DARKEN   },
        { "screen",   BLEND_SCREEN   },
        { "replace",  BLEND_REPLACE  },
        { "none",     BLEND_NONE     },
        { "custom",   BLEND_CUSTOM   }
    );

    STRINGMAP_DECLARE(BlendAlphaModes, BlendAlpha,
        { "alphamultiply", BLENDALPHA_MULTIPLY      },
        { "premultiplied", BLENDALPHA_PREMULTIPLIED }
    );

    STRINGMAP_DECLARE(BlendFactors, BlendFactor,
        { "zero",              BLENDFACTOR_ZERO                 },
        { "one",               BLENDFACTOR_ONE                  },
        { "srccolor",          BLENDFACTOR_SRC_COLOR            },
        { "oneminussrccolor",  BLENDFACTOR_ONE_MINUS_SRC_COLOR  },
        { "srcalpha",          BLENDFACTOR_SRC_ALPHA            },
        { "oneminussrcalpha",  BLENDFACTOR_ONE_MINUS_SRC_ALPHA  },
        { "dstcolor",          BLENDFACTOR_DST_COLOR            },
        { "oneminusdstcolor",  BLENDFACTOR_ONE_MINUS_DST_COLOR  },
        { "dstalpha",          BLENDFACTOR_DST_ALPHA            },
        { "oneminusdstalpha",  BLENDFACTOR_ONE_MINUS_DST_ALPHA  },
        { "srcalphasaturated", BLENDFACTOR_SRC_ALPHA_SATURATED  }
    );

    STRINGMAP_DECLARE(BlendOperations, BlendOperation,
        { "add",             BLENDOP_ADD              },
        { "subtract",        BLENDOP_SUBTRACT         },
        { "reversesubtract", BLENDOP_REVERSE_SUBTRACT },
        { "min",             BLENDOP_MIN              },
        { "max",             BLENDOP_MAX              }
    );

    STRINGMAP_DECLARE(StencilModes, StencilMode,
        { "off",    STENCIL_MODE_OFF    },
        { "draw",   STENCIL_MODE_DRAW   },
        { "test",   STENCIL_MODE_TEST   },
        { "custom", STENCIL_MODE_CUSTOM }
    );

    STRINGMAP_DECLARE(StencilActions, StencilAction,
        { "keep",          STENCIL_KEEP           },
        { "zero",          STENCIL_ZERO           },
        { "replace",       STENCIL_REPLACE        },
        { "increment",     STENCIL_INCREMENT      },
        { "decrement",     STENCIL_DECREMENT      },
        { "incrementwrap", STENCIL_INCREMENT_WRAP },
        { "decrementwrap", STENCIL_DECREMENT_WRAP },
        { "invert",        STENCIL_INVERT         }
    );

    STRINGMAP_DECLARE(CompareModes, CompareMode,
        { "less",     COMPARE_LESS     },
        { "lequal",   COMPARE_LEQUAL   },
        { "equal",    COMPARE_EQUAL    },
        { "gequal",   COMPARE_GEQUAL   },
        { "greater",  COMPARE_GREATER  },
        { "notequal", COMPARE_NOTEQUAL },
        { "always",   COMPARE_ALWAYS   },
        { "never",    COMPARE_NEVER    }
    )
    // clang-format on
} // namespace love
