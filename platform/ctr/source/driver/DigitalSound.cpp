#include "common/Exception.hpp"
#include "common/Result.hpp"

#include "driver/DigitalSound.hpp"

static void audioCallback(void* data)
{
    LightEvent_Signal((LightEvent*)data);
}

namespace love
{
    DigitalSound::~DigitalSound()
    {
        ndspExit();
    }

    void DigitalSound::initialize()
    {
        const auto result = Result(ndspInit());

        if (result.failed(DSP_FIRM_MISSING_ERROR_CODE))
            throw love::Exception(E_AUDIO_NOT_INITIALIZED " (dspfirm.cdc not found)");
        else
            throw love::Exception(E_AUDIO_NOT_INITIALIZED ": %x", result.get());

        LightEvent_Init(&this->event, RESET_ONESHOT);
        ndspSetCallback(audioCallback, &this->event);
    }

    void DigitalSound::updateImpl()
    {
        LightEvent_Wait(&this->event);
    }

    void DigitalSound::setMasterVolume(float volume)
    {
        ndspSetMasterVol(volume);
    }

    float DigitalSound::getMasterVolume() const
    {
        return ndspGetMasterVol();
    }

    // #region Channels

    bool DigitalSound::channelReset(size_t id, int channels, int bitDepth, int sampleRate)
    {
        ndspChnReset(id);

        uint8_t format = 0;
        if ((format = DigitalSound::getNdspFormat(bitDepth, channels)) < 0)
            return false;

        ndspInterpType interpType;

        if (!DigitalSound::getConstant((InterpretedFormat)channels, interpType))
            return false;

        ndspChnSetFormat(id, format);
        ndspChnSetRate(id, sampleRate);
        ndspChnSetInterp(id, interpType);

        this->channelSetVolume(id, this->channelGetVolume(id));

        return true;
    }

    void DigitalSound::channelSetVolume(size_t id, float volume)
    {
        std::array<float, 12> mix { volume, volume };

        ndspChnSetMix(id, mix.data());
    }

    float DigitalSound::channelGetVolume(size_t id) const
    {
        std::array<float, 12> mix;
        ndspChnGetMix(id, mix.data());

        return mix.at(0);
    }

    size_t DigitalSound::channelGetSampleOffset(size_t id) const
    {
        return ndspChnGetSamplePos(id);
    }

    bool DigitalSound::channelAddBuffer(size_t id, Buffer* buffer)
    {
        ndspChnWaveBufAdd(id, buffer);

        return true;
    }

    void DigitalSound::channelPause(size_t id, bool paused)
    {
        ndspChnSetPaused(id, paused);
    }

    bool DigitalSound::isChannelPaused(size_t id) const
    {
        return ndspChnIsPaused(id);
    }

    bool DigitalSound::isChannelPlaying(size_t id) const
    {
        return ndspChnIsPlaying(id);
    }

    void DigitalSound::channelStop(size_t id)
    {
        ndspChnWaveBufClear(id);
    }

    // #endregion

    uint8_t DigitalSound::getNdspFormat(int bitDepth, int channels)
    {
        if (bitDepth != 8 && bitDepth != 16)
            return -1;

        if (channels < 1 && channels > 2)
            return -2;

        NdspEncodingType encoding;
        getConstant((EncodingFormat)bitDepth, encoding);

        return NDSP_CHANNELS(channels) | NDSP_ENCODING(encoding);
    }

} // namespace love
