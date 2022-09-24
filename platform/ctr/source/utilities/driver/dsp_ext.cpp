#include <common/exception.hpp>

#include <utilities/bidirectionalmap.hpp>
#include <utilities/driver/dsp_ext.hpp>

#include <utilities/log/logfile.h>

using namespace love;

static void audioCallback(void* data)
{
    auto event = (LightEvent*)data;
    LightEvent_Signal(event);
}

void DSP<Console::CTR>::Initialize()
{
    if (Result result; R_FAILED(result = ndspInit()))
        throw love::Exception("Failed to initialize ndsp: %x", result);

    this->initialized = true;

    LightEvent_Init(&this->event, RESET_ONESHOT);
    ndspSetCallback(audioCallback, &this->event);
}

DSP<Console::CTR>::~DSP()
{
    if (this->initialized)
        ndspExit();
}

void DSP<Console::CTR>::Update()
{
    LightEvent_Wait(&this->event);
}

void DSP<Console::CTR>::SetMasterVolume(float volume)
{
    ndspSetMasterVol(volume);
}

float DSP<Console::CTR>::GetMasterVolume() const
{
    return 1.0f;
}

bool DSP<Console::CTR>::ChannelReset(size_t id, int channels, int bitDepth, int sampleRate)
{
    ndspChnReset(id);
    uint8_t ndspFormat = 0;

    AudioFormat format = (channels == 2) ? FORMAT_STEREO : FORMAT_MONO;
    DSP<Console::CTR>::GetChannelFormat(format, bitDepth, ndspFormat);

    /* invalid format! */
    if (ndspFormat == 0)
        return false;

    ndspInterpType interpType;

    /* invalid interpType! */
    if (!DSP<Console::CTR>::GetInterpType(format, interpType))
        return false;

    ndspChnSetFormat(id, ndspFormat);
    ndspChnSetRate(id, sampleRate);
    ndspChnSetInterp(id, interpType);

    this->ChannelSetVolume(id, this->ChannelGetVolume(id));

    return true;
}

void DSP<Console::CTR>::ChannelSetVolume(size_t id, float volume)
{
    float mix[12] { 0.0f };
    mix[0] = mix[1] = volume;

    ndspChnSetMix(id, mix);
}

float DSP<Console::CTR>::ChannelGetVolume(size_t id) const
{
    return 1.0f;
}

size_t DSP<Console::CTR>::ChannelGetSampleOffset(size_t id) const
{
    return ndspChnGetSamplePos(id);
}

bool DSP<Console::CTR>::ChannelAddBuffer(size_t id, ndspWaveBuf* buffer)
{
    ndspChnWaveBufAdd(id, buffer);

    return true;
}

void DSP<Console::CTR>::ChannelPause(size_t id, bool paused)
{
    ndspChnSetPaused(id, paused);
}

bool DSP<Console::CTR>::IsChannelPaused(size_t id) const
{
    return ndspChnIsPaused(id);
}

bool DSP<Console::CTR>::IsChannelPlaying(size_t id) const
{
    return ndspChnIsPlaying(id);
}

void DSP<Console::CTR>::ChannelStop(size_t id)
{
    ndspChnWaveBufClear(id);
}

void DSP<Console::CTR>::GetChannelFormat(AudioFormat format, int bitDepth, uint8_t& out)
{
    switch (format)
    {
        case FORMAT_MONO:
        {
            if (bitDepth == 8)
                out = NDSP_FORMAT_MONO_PCM8;
            else
                out = NDSP_FORMAT_MONO_PCM16;

            break;
        }
        case FORMAT_STEREO:
            if (bitDepth == 8)
                out = NDSP_FORMAT_STEREO_PCM8;
            else
                out = NDSP_FORMAT_STEREO_PCM16;

            break;
        default:
            break;
    }
}

bool DSP<Console::CTR>::GetInterpType(AudioFormat format, ndspInterpType& out)
{
    switch (format)
    {
        case FORMAT_MONO:
            out = NDSP_INTERP_LINEAR;
            break;
        case FORMAT_STEREO:
            out = NDSP_INTERP_POLYPHASE;
            break;
        default:
            return false;
    }

    return true;
}
