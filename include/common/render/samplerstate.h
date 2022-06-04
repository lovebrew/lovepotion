#pragma once

#include "common/render/renderstate.h"

#include <limits>
#include <optional>
#include <stdint.h>
#include <vector>

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

        static bool GetConstant(const char* in, FilterMode& out);
        static bool GetConstant(FilterMode in, const char*& out);
        static std::vector<const char*> GetConstants(FilterMode);

        static bool GetConstant(const char* in, WrapMode& out);
        static bool GetConstant(WrapMode in, const char*& out);
        static std::vector<const char*> GetConstants(WrapMode);

        static bool GetConstant(const char* in, MipmapFilterMode& out);
        static bool GetConstant(MipmapFilterMode in, const char*& out);
        static std::vector<const char*> GetConstants(MipmapFilterMode);
    };
} // namespace love
