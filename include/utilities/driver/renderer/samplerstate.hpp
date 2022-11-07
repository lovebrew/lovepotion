#pragma once

#include "renderstate.hpp"

#include <limits>
#include <optional>
#include <vector>

#include <stdint.h>

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
            FILTER_NEAREST,
            FILTER_LINEAR,
            FILTER_MAX_ENUM
        };

        enum MipmapFilterMode
        {
            MIPMAP_FILTER_NONE,
            MIPMAP_FILTER_LINEAR,
            MIPMAP_FILTER_NEAREST,
            MIPMAP_FILTER_MAX_ENUM
        };

        FilterMode minFilter = FILTER_LINEAR;
        FilterMode magFilter = FILTER_LINEAR;

        MipmapFilterMode mipmapFilter = MIPMAP_FILTER_NONE;

        WrapMode wrapU = WRAP_CLAMP;
        WrapMode wrapV = WRAP_CLAMP;
        WrapMode wrapW = WRAP_CLAMP;

        float lodBias = 0.0f;

        uint8_t maxAnisotropy = 1;
        std::optional<RenderState::CompareMode> depthSampleMode;

        uint8_t minLod = 0;
        uint8_t maxLod = std::numeric_limits<uint8_t>::max();

        uint64_t ToKey() const;

        static SamplerState FromKey(uint64_t);

        static bool IsClampZeroOrOne(WrapMode wrap);
    };

    // clang-format off
    static constexpr BidirectionalMap filterModes = {
        "linear",  SamplerState::FILTER_LINEAR,
        "nearest", SamplerState::FILTER_NEAREST
    };

    static constexpr BidirectionalMap wrapModes = {
        "clamp",          SamplerState::WRAP_CLAMP,
        "repeat",         SamplerState::WRAP_REPEAT,
        "clampzero",      SamplerState::WRAP_CLAMP_ZERO,
        "clampone",       SamplerState::WRAP_CLAMP_ONE,
        "mirroredrepeat", SamplerState::WRAP_MIRRORED_REPEAT
    };

    static constexpr BidirectionalMap mipMapFilterModes = {
        "none",    SamplerState::MIPMAP_FILTER_NONE,
        "linear",  SamplerState::MIPMAP_FILTER_LINEAR,
        "nearest", SamplerState::MIPMAP_FILTER_NEAREST
    };
    // clang-format on
} // namespace love
