#pragma once

#include <common/math.hpp>

#include <utilities/bidirectionalmap/bidirectionalmap.hpp>

#include <string>
#include <vector>

namespace love
{
    namespace RenderState
    {
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
            BLENDALPHA_MAX_ENUM
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

            BlendFactor srcFactorRGB = BLENDFACTOR_ONE;
            BlendFactor srcFactorA   = BLENDFACTOR_ONE;

            BlendFactor dstFactorRGB = BLENDFACTOR_ZERO;
            BlendFactor dstFactorA   = BLENDFACTOR_ZERO;

            bool enabled = false;

            BlendState()
            {}

            BlendState(BlendOperation rgb, BlendOperation alpha, BlendFactor srcRGB,
                       BlendFactor srcAlpha, BlendFactor dstRGB, BlendFactor dstAlpha) :
                operationRGB(rgb),
                operationA(alpha),
                srcFactorRGB(srcRGB),
                srcFactorA(srcAlpha),
                dstFactorRGB(dstRGB),
                dstFactorA(dstAlpha)
            {}

            bool operator==(const BlendState& other) const
            {
                return this->enabled == other.enabled &&
                       (this->operationRGB == other.operationRGB) &&
                       (this->operationA == other.operationA) &&
                       (this->srcFactorRGB == other.srcFactorRGB) &&
                       (this->srcFactorA == other.srcFactorA) &&
                       (this->dstFactorRGB == other.dstFactorRGB) &&
                       (this->dstFactorA == other.dstFactorA);
            }
        };

        struct DepthState
        {
            CompareMode mode = COMPARE_ALWAYS;
            bool write       = false;

            bool operator==(const DepthState& other) const
            {
                return this->write == other.write && this->mode == other.mode;
            }
        };

        struct ColorMask
        {
            bool r : 1, g : 1, b : 1, a : 1;

            ColorMask() : r(true), g(true), b(true), a(true)
            {}

            ColorMask(bool _r, bool _g, bool _b, bool _a) : r(_r), g(_g), b(_b), a(_a)
            {}

            bool operator==(const ColorMask& m) const
            {
                return r == m.r && g == m.g && b == m.b && a == m.a;
            }

            bool operator!=(const ColorMask& m) const
            {
                return !(operator==(m));
            }

            uint8_t GetColorMask() const
            {
                return r | (g << 1) | (b << 2) | (a << 3);
            }
        };

        struct ScissorState
        {
            Rect rectangle = { 0, 0, 0, 0 };
            bool enabled   = false;
        };

        BlendState ComputeBlendState(BlendMode mode, BlendAlpha alphaMode);

        BlendMode ComputeBlendMode(BlendState state, BlendAlpha& alpha);

        bool IsAlphaMultiplyBlendSupported(BlendMode mode);

        // clang-format off
        static constexpr BidirectionalMap blendModes = {
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
        };

        static constexpr BidirectionalMap blendAlphaModes = {
            "alphamultiply", BLENDALPHA_MULTIPLY,
            "premultiplied", BLENDALPHA_PREMULTIPLIED
        };

        static constexpr BidirectionalMap blendFactors = {
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
        };

        static constexpr BidirectionalMap blendOperations = {
            "add",             BLENDOP_ADD,
            "subtract",        BLENDOP_SUBTRACT,
            "reversesubtract", BLENDOP_REVERSE_SUBTRACT,
            "min",             BLENDOP_MIN,
            "max",             BLENDOP_MAX
        };
        // clang-format on
    } // namespace RenderState
} // namespace love
