#pragma once

#include "common/Map.hpp"
#include "common/Optional.hpp"
#include "common/int.hpp"

#include "modules/graphics/renderstate.hpp"

namespace love
{
    struct SamplerState
    {
        enum WrapMode
        {
            WRAP_CLAMP,
            WRAP_CLAMP_ZERO,
            WRAP_CLAMP_ONE,
            WRAP_REPEAT,
            WRAP_MIRRORED_REPEAT,
            WRAP_MAX_ENUM
        };

        enum FilterMode
        {
            FILTER_LINEAR,
            FILTER_NEAREST,
            FILTER_MAX_ENUM
        };

        enum MipmapFilterMode
        {
            MIPMAP_FILTER_NONE,
            MIPMAP_FILTER_LINEAR,
            MIPMAP_FILTER_NEAREST,
            MIPMAP_FILTER_MAX_ENUM
        };

        FilterMode minFilter          = FILTER_LINEAR;
        FilterMode magFilter          = FILTER_LINEAR;
        MipmapFilterMode mipmapFilter = MIPMAP_FILTER_NONE;

        WrapMode wrapU = WRAP_CLAMP;
        WrapMode wrapV = WRAP_CLAMP;
        WrapMode wrapW = WRAP_CLAMP;

        float lodBias = 0.0f;

        uint8_t maxAnisotropy = 1;

        uint8_t minLod = 0;
        uint8_t maxLod = LOVE_UINT8_MAX;

        Optional<CompareMode> depthSampleMode;

        uint64_t toKey() const;

        static SamplerState fromKey(uint64_t key);

        static bool isClampZeroOrOne(WrapMode mode)
        {
            return mode == WRAP_CLAMP_ONE || mode == WRAP_CLAMP_ZERO;
        }

        // clang-format off
        STRINGMAP_DECLARE(FilterModes, FilterMode,
            { "linear",  FILTER_LINEAR  },
            { "nearest", FILTER_NEAREST }
        );

        STRINGMAP_DECLARE(MipmapFilterModes, MipmapFilterMode,
            { "none",    MIPMAP_FILTER_NONE    },
            { "linear",  MIPMAP_FILTER_LINEAR  },
            { "nearest", MIPMAP_FILTER_NEAREST }
        );

        STRINGMAP_DECLARE(WrapModes, WrapMode,
            { "clamp",           WRAP_CLAMP           },
            { "clampzero",       WRAP_CLAMP_ZERO      },
            { "clampone",        WRAP_CLAMP_ONE       },
            { "repeat",          WRAP_REPEAT          },
            { "mirroredrepeat",  WRAP_MIRRORED_REPEAT }
        );
        // clang-format on
    };
} // namespace love
