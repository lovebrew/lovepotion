#include "common/render/samplerstate.h"

using namespace love;

uint64_t SamplerState::ToKey() const
{
    union
    {
        float f;
        uint32_t i;
    } conv;

    conv.f = lodBias;
    return (minFilter << 0) | (magFilter << 1) | (mipmapFilter << 2) | (wrapU << 4) | (wrapV << 7) |
           (wrapW << 10) | (maxAnisotropy << 12) | (minLod << 16) | (maxLod << 20) |
           (depthSampleMode.has_value() << 24) | (depthSampleMode.value() << 25) |
           ((uint64_t)conv.i << 32);
}

SamplerState SamplerState::FromKey(uint64_t key)
{
    const uint32_t BITS_1 = 0x1;
    const uint32_t BITS_2 = 0x3;
    const uint32_t BITS_3 = 0x7;
    const uint32_t BITS_4 = 0xF;

    SamplerState state;

    state.minFilter    = (FilterMode)((key >> 0) & BITS_1);
    state.magFilter    = (FilterMode)((key >> 1) & BITS_1);
    state.mipmapFilter = (MipmapFilterMode)((key >> 2) & BITS_2);

    state.wrapU = (WrapMode)((key >> 4) & BITS_3);
    state.wrapV = (WrapMode)((key >> 7) & BITS_3);
    state.wrapW = (WrapMode)((key >> 10) & BITS_3);

    state.maxAnisotropy = (key >> 12) & BITS_4;

    state.minLod = (key >> 16) & BITS_4;
    state.maxLod = (key >> 20) & BITS_4;

    state.depthSampleMode.has_value() = ((key >> 24) & BITS_1) != 0;
    state.depthSampleMode.value()     = (RenderState::CompareMode)((key >> 25) & BITS_4);

    union
    {
        float f;
        uint32_t i;
    } conv;

    conv.i        = (uint32_t)(key >> 32);
    state.lodBias = conv.f;

    return state;
}

bool SamplerState::IsClampZeroOrOne(WrapMode wrap)
{
    return wrap == WRAP_CLAMP_ONE || wrap == WRAP_CLAMP_ZERO;
}

// clang-format off
constexpr auto filterModes = BidirectionalMap<>::Create(
    "linear",  SamplerState::FILTER_LINEAR,
    "nearest", SamplerState::FILTER_NEAREST
);

constexpr auto wrapModes = BidirectionalMap<>::Create(
    "clamp",          SamplerState::WRAP_CLAMP,
    "repeat",         SamplerState::WRAP_REPEAT,
    "clampzero",      SamplerState::WRAP_CLAMP_ZERO,
    "clampone",       SamplerState::WRAP_CLAMP_ONE,
    "mirroredrepeat", SamplerState::WRAP_MIRRORED_REPEAT
);

constexpr auto mipMapFilterModes = BidirectionalMap<>::Create(
    "none",    SamplerState::MIPMAP_FILTER_NONE,
    "linear",  SamplerState::MIPMAP_FILTER_LINEAR,
    "nearest", SamplerState::MIPMAP_FILTER_NEAREST
);
// clang-format on

// FilterMode
bool SamplerState::GetConstant(const char* in, FilterMode& out)
{
    return filterModes.Find(in, out);
}

bool SamplerState::GetConstant(FilterMode in, const char*& out)
{
    return filterModes.ReverseFind(in, out);
}

std::vector<const char*> SamplerState::GetConstants(FilterMode)
{
    return filterModes.GetNames();
}

// WrapMode
bool SamplerState::GetConstant(const char* in, WrapMode& out)
{
    return wrapModes.Find(in, out);
}

bool SamplerState::GetConstant(WrapMode in, const char*& out)
{
    return wrapModes.ReverseFind(in, out);
}

std::vector<const char*> SamplerState::GetConstants(WrapMode)
{
    return wrapModes.GetNames();
}

// MipmapFilterMode
bool SamplerState::GetConstant(const char* in, MipmapFilterMode& out)
{
    return mipMapFilterModes.Find(in, out);
}

bool SamplerState::GetConstant(MipmapFilterMode in, const char*& out)
{
    return mipMapFilterModes.ReverseFind(in, out);
}

std::vector<const char*> SamplerState::GetConstants(MipmapFilterMode)
{
    return mipMapFilterModes.GetNames();
}
