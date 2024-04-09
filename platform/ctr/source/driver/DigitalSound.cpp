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

        if (result.failed())
            throw love::Exception(E_AUDIO_NOT_INITIALIZED ": {:x}", result.get());

        LightEvent_Init(&this->event, RESET_ONESHOT);
        ndspSetCallback(audioCallback, &this->event);

        for (size_t index = 0; index < 24; index++)
            this->channelSetVolume(index, 1.0f);
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

    AudioBuffer* DigitalSound::createBuffer(int size)
    {
        AudioBuffer* buffer = new AudioBuffer();

        if (size != 0)
        {
            buffer->data_pcm16 = (int16_t*)linearAlloc(size);

            if (buffer->data_pcm16 == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
        }
        else
            buffer->data_pcm16 = nullptr;

        buffer->status = NDSP_WBUF_DONE;

        return buffer;
    }

    bool DigitalSound::isBufferDone(AudioBuffer* buffer) const
    {
        if (buffer == nullptr)
            return false;

        return buffer->status == NDSP_WBUF_DONE;
    }

    void DigitalSound::prepareBuffer(AudioBuffer* buffer, size_t nsamples, const void* data,
                                     size_t size, bool looping)
    {
        if (buffer == nullptr || data == nullptr)
            return;

        if (buffer->data_pcm16 != nullptr)
            std::copy_n((int16_t*)data, size, buffer->data_pcm16);
        else
            buffer->data_pcm16 = (int16_t*)data;

        DSP_FlushDataCache(buffer->data_pcm16, size);

        buffer->nsamples = nsamples;
        buffer->looping  = looping;
    }

    void DigitalSound::setLooping(AudioBuffer* buffer, bool looping)
    {
        if (buffer == nullptr)
            return;

        buffer->looping = looping;
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
