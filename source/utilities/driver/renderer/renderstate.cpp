#include <utilities/driver/renderer/renderstate.hpp>

namespace love
{
    namespace RenderState
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
    } // namespace RenderState
} // namespace love
