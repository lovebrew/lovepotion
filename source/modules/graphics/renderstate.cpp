#include "modules/graphics/renderstate.hpp"

namespace love
{
    // These are all with premultiplied alpha. computeBlendState adjusts for
    // alpha-multiply if needed.
    // clang-format off
    static const BlendState blendStates[BLEND_MAX_ENUM] =
    {
        // BLEND_ALPHA
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE_MINUS_SRC_ALPHA, BLENDFACTOR_ONE_MINUS_SRC_ALPHA},

        // BLEND_ADD
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ZERO, BLENDFACTOR_ONE, BLENDFACTOR_ONE},

        // BLEND_SUBTRACT
        {BLENDOP_REVERSE_SUBTRACT, BLENDOP_REVERSE_SUBTRACT, BLENDFACTOR_ONE, BLENDFACTOR_ZERO, BLENDFACTOR_ONE, BLENDFACTOR_ONE},

        // BLEND_MULTIPLY
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_DST_COLOR, BLENDFACTOR_DST_COLOR, BLENDFACTOR_ZERO, BLENDFACTOR_ZERO},

        // BLEND_LIGHTEN
        {BLENDOP_MAX, BLENDOP_MAX, BLENDFACTOR_ZERO, BLENDFACTOR_ZERO, BLENDFACTOR_ONE, BLENDFACTOR_ONE},

        // BLEND_DARKEN
        {BLENDOP_MAX, BLENDOP_MAX, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE},

        // BLEND_SCREEN
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ONE_MINUS_SRC_COLOR, BLENDFACTOR_ONE_MINUS_SRC_COLOR},

        // BLEND_REPLACE
        {BLENDOP_ADD, BLENDOP_ADD, BLENDFACTOR_ONE, BLENDFACTOR_ONE, BLENDFACTOR_ZERO, BLENDFACTOR_ZERO},

        // BLEND_NONE
        {},

        // BLEND_CUSTOM - N/A
        {},
    };
    // clang-format on

    BlendState computeBlendState(BlendMode mode, BlendAlpha alphaMode)
    {
        BlendState state = blendStates[mode];

        // clang-format off
        if (state.srcFactorRGB == BLENDFACTOR_ONE && alphaMode == BLENDALPHA_MULTIPLY && mode != BLEND_NONE)
            state.srcFactorRGB = BLENDFACTOR_SRC_ALPHA;
        // clang-format on

        return state;
    }

    BlendMode computeBlendMode(BlendState state, BlendAlpha& alphaMode)
    {
        if (!state.enable)
        {
            alphaMode = BLENDALPHA_PREMULTIPLIED;
            return BLEND_NONE;
        }

        // Temporarily disable alpha multiplication when comparing to our list.
        bool alphamultiply = state.srcFactorRGB == BLENDFACTOR_SRC_ALPHA;
        if (alphamultiply)
            state.srcFactorRGB = BLENDFACTOR_ONE;

        for (int i = 0; i < (int)BLEND_MAX_ENUM; i++)
        {
            if (i != (int)BLEND_CUSTOM && blendStates[i] == state)
            {
                alphaMode = alphamultiply ? BLENDALPHA_MULTIPLY : BLENDALPHA_PREMULTIPLIED;
                return (BlendMode)i;
            }
        }

        alphaMode = BLENDALPHA_PREMULTIPLIED;
        return BLEND_CUSTOM;
    }

    bool isAlphaMultiplyBlendSupported(BlendMode mode)
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

    static const StencilState stencilStates[STENCIL_MODE_MAX_ENUM] = {
        // STENCIL_MODE_OFF
        {},

        // STENCIL_MODE_DRAW
        { COMPARE_ALWAYS, STENCIL_REPLACE },

        // STENCIL_MODE_TEST
        { COMPARE_EQUAL, STENCIL_KEEP },

        // STENCIL_MODE_CUSTOM - N/A
        {},
    };

    StencilState computeStencilState(StencilMode mode, int value)
    {
        StencilState state = stencilStates[mode];
        state.value        = value;

        return state;
    }

    StencilMode computeStencilMode(const StencilState& state)
    {
        for (int i = 0; i < (int)STENCIL_MODE_MAX_ENUM; i++)
        {
            if (stencilStates[i].action == state.action &&
                stencilStates[i].compare == state.compare)
            {
                return (StencilMode)i;
            }
        }

        return STENCIL_MODE_CUSTOM;
    }

    CompareMode getReversedCompareMode(CompareMode mode)
    {
        switch (mode)
        {
            case COMPARE_LESS:
                return COMPARE_GREATER;
            case COMPARE_LEQUAL:
                return COMPARE_GEQUAL;
            case COMPARE_GEQUAL:
                return COMPARE_LEQUAL;
            case COMPARE_GREATER:
                return COMPARE_LESS;
            default:
                return mode;
        }
    }
} // namespace love
