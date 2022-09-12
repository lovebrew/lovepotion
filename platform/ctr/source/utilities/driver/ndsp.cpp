#include <common/exception.hpp>

#include <utilities/bidirectionalmap.hpp>
#include <utilities/driver/ndsp.hpp>

using namespace love;

static void audioCallback(void* data)
{
    auto event = (LightEvent*)data;
    LightEvent_Signal(event);
}

NDSP::NDSP()
{
    if (Result result; R_FAILED(result = ndspInit()))
        throw love::Exception("Failed to initialize ndsp: %x", result);

    this->initialized = true;

    LightEvent_Init(&this->event, RESET_ONESHOT);
    ndspSetCallback(audioCallback, &this->event);
}

NDSP::~NDSP()
{
    if (this->initialized)
        ndspExit();
}

void NDSP::SetMasterVolume(float volume)
{
    ndspSetMasterVol(volume);
}

float NDSP::GetMasterVolume() const
{
    return 0.0f;
}

bool NDSP::ChannelReset(size_t id, int channels, AudioFormat format, int bitDepth, int sampleRate)
{
    ndspChnReset(id);
    uint8_t ndspFormat = 0;

    NDSP::GetChannelFormat(format, bitDepth, ndspFormat);

    /* invalid format! */
    if (ndspFormat == 0)
        return false;

    ndspInterpType interpType;

    /* invalid interpType! */
    if (!NDSP::GetInterpType(format, interpType))
        return false;

    ndspChnSetFormat(id, ndspFormat);
    ndspChnSetRate(id, sampleRate);
    ndspChnSetInterp(id, interpType);

    this->ChannelSetVolume(id, this->ChannelGetVolume(id));

    return true;
}

void NDSP::ChannelSetVolume(size_t id, float volume)
{
    float mix[12] { 0.0f };
    mix[0] = mix[1] = volume;

    ndspChnSetMix(id, mix);
}

float NDSP::ChannelGetVolume(size_t id) const
{
    return 0.0f;
}

bool NDSP::ChannelAddBuffer(size_t id, ndspWaveBuf* buffer)
{
    ndspChnWaveBufAdd(id, buffer);

    return true;
}

void NDSP::ChannelPause(size_t id, bool paused)
{
    ndspChnSetPaused(id, paused);
}

bool NDSP::IsChannelPaused(size_t id) const
{
    return ndspChnIsPaused(id);
}

bool NDSP::IsChannelPlaying(size_t id) const
{
    return ndspChnIsPlaying(id);
}

void NDSP::ChannelStop(size_t id)
{
    ndspChnWaveBufClear(id);
}

size_t NDSP::ChannelGetSampleOffset(size_t id) const
{
    return ndspChnGetSamplePos(id);
}

void NDSP::GetChannelFormat(AudioFormat format, int bitDepth, uint8_t& out)
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

bool NDSP::GetInterpType(AudioFormat format, ndspInterpType& out)
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
