#include "modules/graphics/samplerstate.hpp"

namespace love
{
    // #region SamplerState

    uint64_t SamplerState::toKey() const
    {
        union
        {
            float f;
            uint32_t i;
        } conv;

        conv.f = lodBias;

        const uint32_t BITS_4 = 0xF;

        return (minFilter << 0) | (magFilter << 1) | (mipmapFilter << 2) | (wrapU << 4) | (wrapV << 7) |
               (wrapW << 10) | (maxAnisotropy << 13) | ((BITS_4 & minLod) << 17) | ((BITS_4 & maxLod) << 21) |
               (depthSampleMode.hasValue << 25) | (depthSampleMode.value << 26) | ((uint64_t)conv.i << 32);
    }

    SamplerState SamplerState::fromKey(uint64_t key)
    {
        const uint32_t BITS_1 = 0x1;
        const uint32_t BITS_2 = 0x3;
        const uint32_t BITS_3 = 0x7;
        const uint32_t BITS_4 = 0xF;

        SamplerState s;

        s.minFilter    = (FilterMode)((key >> 0) & BITS_1);
        s.magFilter    = (FilterMode)((key >> 1) & BITS_1);
        s.mipmapFilter = (MipmapFilterMode)((key >> 2) & BITS_2);

        s.wrapU = (WrapMode)((key >> 4) & BITS_3);
        s.wrapV = (WrapMode)((key >> 7) & BITS_3);
        s.wrapW = (WrapMode)((key >> 10) & BITS_3);

        s.maxAnisotropy = (key >> 13) & BITS_4;

        s.minLod = (key >> 17) & BITS_4;
        s.maxLod = (key >> 21) & BITS_4;

        s.depthSampleMode.hasValue = ((key >> 25) & BITS_1) != 0;
        s.depthSampleMode.value    = (CompareMode)((key >> 26) & BITS_4);

        union
        {
            float f;
            uint32_t i;
        } conv;

        conv.i    = (uint32_t)(key >> 32);
        s.lodBias = conv.f;

        return s;
    }
} // namespace love
