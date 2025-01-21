#include "common/Exception.hpp"
#include "common/Result.hpp"

#include "driver/audio/DigitalSound.hpp"

#include <cstring>

static void audioCallback(void* data)
{
    LightEvent_Signal((LightEvent*)data);
}

namespace love
{
    DigitalSound::~DigitalSound()
    {}

    void DigitalSound::initialize()
    {
        const auto result = Result(ndspInit());

        if (result.failed(DSP_FIRM_MISSING_ERROR_CODE))
            throw love::Exception(E_AUDIO_NOT_INITIALIZED " (dspfirm.cdc not found)");

        if (result.failed())
            throw love::Exception(E_AUDIO_NOT_INITIALIZED ": {:x}", result.get());

        LightEvent_Init(&this->event, RESET_ONESHOT);
        ndspSetCallback(audioCallback, &this->event);

        for (size_t index = 0; index < 24; index++)
            this->channelSetVolume(index, 1.0f);
    }

    void DigitalSound::deInitialize()
    {
        ndspExit();
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

    AudioBuffer DigitalSound::createBuffer(int size, int)
    {
        AudioBuffer buffer {};

        if (size != 0)
        {
            buffer.data_pcm16 = (int16_t*)linearAlloc(size);

            if (buffer.data_pcm16 == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
        }
        else
            throw love::Exception("Size cannot be zero.");

        return buffer;
    }

    void DigitalSound::freeBuffer(const AudioBuffer& buffer)
    {
        linearFree(buffer.data_pcm16);
    }

    bool DigitalSound::isBufferDone(const AudioBuffer& buffer) const
    {
        return buffer.status == NDSP_WBUF_DONE;
    }

    void DigitalSound::prepare(AudioBuffer& buffer, const void* data, size_t size, int samples)
    {
        if (data == nullptr)
            return;

        buffer.nsamples = samples;
        std::memcpy(buffer.data_pcm16, data, size);

        DSP_FlushDataCache(buffer.data_pcm16, size);
    }

    size_t DigitalSound::getSampleCount(const AudioBuffer& buffer) const
    {
        return buffer.nsamples;
    }

    void DigitalSound::setLooping(AudioBuffer& buffer, bool looping)
    {
        buffer.looping = looping;
    }

    // #region Channels

    bool DigitalSound::channelReset(size_t id, int channels, int bitDepth, int sampleRate)
    {
        ndspChnReset(id);

        int8_t format = 0;
        if ((format = DigitalSound::getFormat(channels, bitDepth)) < 0)
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

    bool DigitalSound::channelAddBuffer(size_t id, AudioBuffer* buffer)
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

    int8_t DigitalSound::getFormat(int channels, int bitDepth)
    {
        if (bitDepth != 8 && bitDepth != 16)
            return -1;

        if (channels < 1 || channels > 2)
            return -2;

        NdspEncodingType encoding;
        getConstant((EncodingFormat)bitDepth, encoding);

        return NDSP_CHANNELS(channels) | NDSP_ENCODING(encoding);
    }

} // namespace love
