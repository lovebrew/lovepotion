#include <common/exception.hpp>

#include <utilities/driver/dsp_ext.hpp>
#include <utilities/driver/dsp_mem.hpp>

using namespace love;

static constexpr AudioRendererConfig config = {
    .output_rate     = AudioRendererOutputRate_48kHz,
    .num_voices      = 24,
    .num_effects     = 0,
    .num_sinks       = 1,
    .num_mix_objs    = 1,
    .num_mix_buffers = 2,
};

static constexpr uint8_t sinkChannels[2] = { 0, 1 };

DSP<Console::HAC>::DSP() : channelReset(false), driver {}
{}

void DSP<Console::HAC>::Initialize()
{
    if (bool result = AudioMemory::InitMemPool(); !result)
        throw love::Exception("Failed to create audio memory pool!");

    if (Result result = audrenInitialize(&config); R_FAILED(result))
        throw love::Exception("Failed to initialize audren: %x", result);

    if (Result result = audrvCreate(&this->driver, &config, 2); R_FAILED(result))
        throw love::Exception("Failed to create audio driver: %x", result);

    this->initialized = true;

    int poolId = audrvMemPoolAdd(&this->driver, AudioMemory::POOL_BASE, AudioMemory::POOL_SIZE);

    if (poolId == -1)
        throw love::Exception("Failed to add memory pool!");

    bool attached = audrvMemPoolAttach(&this->driver, poolId);

    if (!attached)
        throw love::Exception("Failed to attach memory pool!");

    int sinkId = audrvDeviceSinkAdd(&this->driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sinkChannels);

    if (sinkId == -1)
        throw love::Exception("Failed to add sink to driver!");

    if (Result result = audrvUpdate(&this->driver); R_FAILED(result))
        throw love::Exception("Failed to update audio driver: %x", result);

    if (Result result = audrenStartAudioRenderer(); R_FAILED(result))
        throw love::Exception("Failed to start audio renderer: %x", result);
}

DSP<Console::HAC>::~DSP()
{
    if (!this->initialized)
        return;

    audrvClose(&this->driver);
    audrenExit();
}

void DSP<Console::HAC>::Update()
{
    std::unique_lock lock(this->mutex);

    audrvUpdate(&this->driver);
}

void DSP<Console::HAC>::SetMasterVolume(float volume)
{
    std::unique_lock lock(this->mutex);

    for (int mix = 0; mix < 2; mix++)
        audrvMixSetVolume(&this->driver, mix, volume);
}

float DSP<Console::HAC>::GetMasterVolume() const
{
    return this->driver.in_mixes[0].volume;
}

bool DSP<Console::HAC>::ChannelReset(size_t channel, int channels, int bitDepth, int sampleRate)
{
    std::unique_lock lock(this->mutex);

    PcmFormat format = (bitDepth == 8) ? PcmFormat_Int8 : PcmFormat_Int16;

    this->channelReset = audrvVoiceInit(&this->driver, channel, channels, format, sampleRate);

    if (this->channelReset)
    {
        audrvVoiceSetDestinationMix(&this->driver, channel, AUDREN_FINAL_MIX_ID);
        audrvVoiceSetMixFactor(&this->driver, channel, 1.0f, 0, 0);

        if (channels == 2)
            audrvVoiceSetMixFactor(&this->driver, channel, 1.0f, 0, 1);
    }

    return this->channelReset;
}

void DSP<Console::HAC>::ChannelSetVolume(size_t channel, float volume)
{
    std::unique_lock lock(this->mutex);

    audrvVoiceSetVolume(&this->driver, channel, volume);
}

float DSP<Console::HAC>::ChannelGetVolume(size_t channel) const
{
    this->driver.in_voices[channel].volume;
}

size_t DSP<Console::HAC>::ChannelGetSampleOffset(size_t channel)
{
    std::unique_lock lock(this->mutex);

    return audrvVoiceGetPlayedSampleCount(&this->driver, channel);
}

bool DSP<Console::HAC>::ChannelAddBuffer(size_t channel, AudioDriverWaveBuf* buffer)
{
    if (this->channelReset)
    {
        std::unique_lock lock(this->mutex);

        bool success = audrvVoiceAddWaveBuf(&this->driver, channel, buffer);

        if (success)
            audrvVoiceStart(&this->driver, channel);

        return success;
    }

    return false;
}

void DSP<Console::HAC>::ChannelPause(size_t channel, bool pause)
{
    std::unique_lock lock(this->mutex);

    audrvVoiceSetPaused(&this->driver, channel, pause);
}

bool DSP<Console::HAC>::IsChannelPaused(size_t channel)
{
    std::unique_lock lock(this->mutex);

    return audrvVoiceIsPaused(&this->driver, channel);
}

bool DSP<Console::HAC>::IsChannelPlaying(size_t channel)
{
    std::unique_lock lock(this->mutex);

    return audrvVoiceIsPlaying(&this->driver, channel);
}

void DSP<Console::HAC>::ChannelStop(size_t channel)
{
    std::unique_lock lock(this->mutex);

    audrvVoiceStop(&this->driver, channel);
    audrvVoiceDrop(&this->driver, channel);
}
