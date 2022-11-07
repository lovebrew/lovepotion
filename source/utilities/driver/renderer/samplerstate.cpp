#include <utilities/driver/renderer/samplerstate.hpp>

namespace love
{
    uint64_t SamplerState::ToKey() const
    {
        union
        {
            float f;
            uint32_t i;
        } convert;

        convert.f = this->lodBias;

        const uint32_t BITS_FOUR = 0x0F;

        return (this->minFilter << 0) | (this->magFilter << 1) | (this->mipmapFilter << 2) |
               (this->wrapU << 4) | (this->wrapV << 7) | (this->wrapW << 10) |
               (this->maxAnisotropy << 13) | ((BITS_FOUR & this->minLod) << 17) |
               ((BITS_FOUR & this->maxLod) << 21) | (this->depthSampleMode.has_value() << 25) |
               (this->depthSampleMode.value() << 26) | ((uint64_t)convert.i << 32);
    }

    SamplerState SamplerState::FromKey(uint64_t key)
    {
        const uint32_t BITS_1 = 0x1;
        const uint32_t BITS_2 = 0x3;
        const uint32_t BITS_3 = 0x7;
        const uint32_t BITS_4 = 0xF;

        SamplerState state {};

        state.minFilter    = (FilterMode)((key >> 0) & BITS_1);
        state.magFilter    = (FilterMode)((key >> 1) & BITS_1);
        state.mipmapFilter = (MipmapFilterMode)((key >> 2) & BITS_2);

        state.wrapU = (WrapMode)((key >> 4) & BITS_3);
        state.wrapV = (WrapMode)((key >> 7) & BITS_3);
        state.wrapW = (WrapMode)((key >> 10) & BITS_3);

        state.maxAnisotropy = (key >> 13) & BITS_4;

        state.minLod = (key >> 17) & BITS_4;
        state.maxLod = (key >> 21) & BITS_4;

        state.depthSampleMode.value() = (RenderState::CompareMode)((key >> 26) & BITS_4);

        union
        {
            float f;
            uint32_t i;
        } convert;

        convert.i     = (uint32_t)(key >> 32);
        state.lodBias = convert.f;

        return state;
    }

    bool SamplerState::IsClampZeroOrOne(WrapMode mode)
    {
        return mode == WRAP_CLAMP_ONE || mode == WRAP_CLAMP_ZERO;
    }
} // namespace love
