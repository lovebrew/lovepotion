#include "common/render/renderstate.h"

namespace love::RenderState
{
    // clang-format off
    static const BlendState states[BLEND_MAX_ENUM]
    {
        /* BlendOp RGB,            BlendOp Alpha,            BlendFactor srcRGB,    BlendFactor srcAlpha,  BlendFactor dstRGB,              blendFactor dstAlpha */
        {BLENDOP_ADD,              BLENDOP_ADD,              BLENDFACTOR_ONE,       BLENDFACTOR_ONE,       BLENDFACTOR_ONE_MINUS_SRC_ALPHA, BLENDFACTOR_ONE_MINUS_SRC_ALPHA }, // BLEND_ALPHA
        {BLENDOP_ADD,              BLENDOP_ADD,              BLENDFACTOR_ONE,       BLENDFACTOR_ZERO,      BLENDFACTOR_ONE,                 BLENDFACTOR_ONE                 }, // BLEND_ADD
        {BLENDOP_REVERSE_SUBTRACT, BLENDOP_REVERSE_SUBTRACT, BLENDFACTOR_ONE,       BLENDFACTOR_ZERO,      BLENDFACTOR_ONE,                 BLENDFACTOR_ONE                 }, // BLEND_SUBTRACT
        {BLENDOP_ADD,              BLENDOP_ADD,              BLENDFACTOR_DST_COLOR, BLENDFACTOR_DST_COLOR, BLENDFACTOR_ZERO,                BLENDFACTOR_ZERO                }, // BLEND_MULTIPLY
        {BLENDOP_MAX,              BLENDOP_MAX,              BLENDFACTOR_ZERO,      BLENDFACTOR_ZERO,      BLENDFACTOR_ONE,                 BLENDFACTOR_ONE                 }, // BLEND_LIGHTEN

        // BLEND_DARKEN
        {BLENDOP_MAX, BLENDOP_MAX, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE},

        // BLEND_SCREEN
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE_MINUS_SRC_COLOR, BLENDFACTOR_ONE_MINUS_SRC_COLOR},

        // BLEND_REPLACE
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ZERO, BLENDFACTOR_ZERO},

        // BLEND_NONE
        {},

        // BLEND_CUSTOM
        {}
    };
    // clang-format on

    BlendState ComputeBlendState(BlendMode mode, BlendAlpha alphaMode)
    {
        BlendState state = states[mode];

        if (state.srcFactorRGB == BLENDFACTOR_ONE && alphaMode == BLENDALPHA_MULTIPLY &&
            mode != BLEND_NONE)
        {
            state.srcFactorRGB = BLENDFACTOR_SRC_ALPHA;
        }

        return state;
    }

    BlendMode ComputeBlendMode(BlendState state, BlendAlpha& alphaMode)
    {
        if (!state.enabled)
        {
            alphaMode = BLENDALPHA_PREMULTIPLIED;

            return BLEND_NONE;
        }

        /* disable alpha multiply in comparisons */
        bool alphaMultiply = (state.srcFactorRGB == BLENDFACTOR_SRC_ALPHA);

        if (alphaMultiply)
            state.srcFactorRGB = BLENDFACTOR_ONE;

        for (size_t index = 0; index < BLEND_MAX_ENUM; index++)
        {
            if (index != BLEND_CUSTOM && states[index] == state)
            {
                alphaMode = alphaMultiply ? BLENDALPHA_MULTIPLY : BLENDALPHA_PREMULTIPLIED;

                return static_cast<BlendMode>(index);
            }
        }

        alphaMode = BLENDALPHA_PREMULTIPLIED;

        return BLEND_CUSTOM;
    }

    bool IsAlphaMultiplyBlendSupported(BlendMode mode)
    {
        switch (mode)
        {
            case BLEND_LIGHTEN:
            case BLEND_DARKEN:
            case BLEND_MULTIPLY:
                return false;
            default:
                return true;
        }
    }

    // clang-format off
    constexpr auto blendModes = BidirectionalMap<>::Create(
        "alpha",    BLEND_ALPHA,
        "add",      BLEND_ADD,
        "subtract", BLEND_SUBTRACT,
        "multiply", BLEND_MULTIPLY,
        "lighten",  BLEND_LIGHTEN,
        "darken",   BLEND_DARKEN,
        "screen",   BLEND_SCREEN,
        "replace",  BLEND_REPLACE,
        "none",     BLEND_NONE,
        "custom",   BLEND_CUSTOM
    );

    constexpr auto blendAlphaModes = BidirectionalMap<>::Create(
        "alphamultiply", BLENDALPHA_MULTIPLY,
        "premultiplied", BLENDALPHA_PREMULTIPLIED
    );

    constexpr auto blendFactors = BidirectionalMap<>::Create(
        "zero",              BLENDFACTOR_ZERO,
        "one",               BLENDFACTOR_ONE,
        "srccolor",          BLENDFACTOR_SRC_COLOR,
        "oneminussrccolor",  BLENDFACTOR_ONE_MINUS_SRC_COLOR,
        "srcalpha",          BLENDFACTOR_SRC_ALPHA,
        "oneminussrcalpha",  BLENDFACTOR_ONE_MINUS_SRC_ALPHA,
        "dstcolor",          BLENDFACTOR_DST_COLOR,
        "oneminusdstcolor",  BLENDFACTOR_ONE_MINUS_DST_COLOR,
        "dstalpha",          BLENDFACTOR_DST_ALPHA,
        "oneminusdstalpha",  BLENDFACTOR_ONE_MINUS_DST_ALPHA,
        "srcalphasaturated", BLENDFACTOR_SRC_ALPHA_SATURATED
    );

    constexpr auto blendOperations = BidirectionalMap<>::Create(
        "add",             BLENDOP_ADD,
        "subtract",        BLENDOP_SUBTRACT,
        "reversesubtract", BLENDOP_REVERSE_SUBTRACT,
        "min",             BLENDOP_MIN,
        "max",             BLENDOP_MAX
    );
    // clang-format on

    // Blend Mode
    bool GetConstant(const char* in, BlendMode& out)
    {
        return blendModes.Find(in, out);
    }

    bool GetConstant(BlendMode in, const char*& out)
    {
        return blendModes.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(BlendMode)
    {
        return blendModes.GetNames();
    }

    // Blend Alpha
    bool GetConstant(const char* in, BlendAlpha& out)
    {
        return blendAlphaModes.Find(in, out);
    }

    bool GetConstant(BlendAlpha in, const char*& out)
    {
        return blendAlphaModes.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(BlendAlpha)
    {
        return blendAlphaModes.GetNames();
    }

    // Blend Operation
    bool GetConstant(const char* in, BlendOperation& out)
    {
        return blendOperations.Find(in, out);
    }

    bool GetConstant(BlendOperation in, const char*& out)
    {
        return blendOperations.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(BlendOperation)
    {
        return blendOperations.GetNames();
    }

    // Blend Factor
    bool GetConstant(const char* in, BlendFactor& out)
    {
        return blendFactors.Find(in, out);
    }

    bool GetConstant(BlendFactor in, const char*& out)
    {
        return blendFactors.ReverseFind(in, out);
    }

    std::vector<const char*> GetConstants(BlendFactor)
    {
        return blendFactors.GetNames();
    }
} // namespace love::RenderState
