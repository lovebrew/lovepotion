#include <common/exception.hpp>

#include <utilities/driver/dsp_ext.hpp>
#include <utilities/driver/dsp_mix.hpp>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/drcvs.h>

#include <utilities/log/logfile.h>

using namespace love;

// clang-format off
static AXInitParams params = {
    .renderer = AX_INIT_RENDERER_48KHZ,
    .pipeline = AX_INIT_PIPELINE_FOUR_STAGE
};
// clang-format on

static void audioCallback()
{
    DSP<Console::CAFE>::Instance().SignalEvent();
    DSP<Console::CAFE>::Instance().UpdateChannels();
}

DSP<Console::CAFE>::DSP()
{}

void DSP<Console::CAFE>::Initialize()
{
    if (!AXIsInit())
        AXInitWithParams(&params);

    if (!AXIsInit())
        throw love::Exception("Failed to initialize AX!");

    OSInitEvent(&this->event, false, OS_EVENT_MODE_AUTO);
    AXRegisterFrameCallback(audioCallback);

    std::fill_n(this->axChannel, 0x60, AXChannel {});

    this->initialized = true;
}

DSP<Console::CAFE>::~DSP()
{
    if (!this->initialized)
        return;

    AXQuit();
}

void DSP<Console::CAFE>::Update()
{
    OSWaitEvent(&this->event);
}

void DSP<Console::CAFE>::SignalEvent()
{
    OSSignalEvent(&this->event);
}

void DSP<Console::CAFE>::SetMasterVolume(float volume)
{
    AXSetMasterVolume(volume * 0x8000);
}

float DSP<Console::CAFE>::GetMasterVolume() const
{
    int16_t volume = AXGetMasterVolume();

    return volume / (float)0x8000;
}

bool DSP<Console::CAFE>::ChannelReset(size_t id, int channels, int bitDepth, int sampleRate)
{
    AXChannel* channel = &axChannel[id];

    if (!channel->voice)
        channel->voice = AXAcquireVoice(31, nullptr, nullptr);

    channel->channels   = channels;
    channel->bitDepth   = bitDepth;
    channel->sampleRate = sampleRate;

    AXVoiceBegin(channel->voice);
    AXSetVoiceType(channel->voice, 0);

    AXVoiceVeData data {};
    data.volume = 0x8000;

    AXSetVoiceVe(channel->voice, &data);

    switch (channels)
    {
        case 1:
        {
            AXSetVoiceDeviceMix(channel->voice, AX_DEVICE_TYPE_TV, 0, MONO_MIX[0]);
            AXSetVoiceDeviceMix(channel->voice, AX_DEVICE_TYPE_DRC, 0, MONO_MIX[0]);
            break;
        }
        case 2:
        default:
            AXSetVoiceDeviceMix(channel->voice, AX_DEVICE_TYPE_TV, 0, STEREO_MIX);
            AXSetVoiceDeviceMix(channel->voice, AX_DEVICE_TYPE_DRC, 0, STEREO_MIX);
    }

    float ratio = (sampleRate / (float)AXGetInputSamplesPerSec()) * channels;

    AXSetVoiceSrcRatio(channel->voice, ratio);
    AXSetVoiceSrcType(channel->voice, AX_VOICE_SRC_TYPE_LINEAR);

    AXVoiceEnd(channel->voice);

    return true;
}

void DSP<Console::CAFE>::ChannelSetVolume(size_t id, float volume)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceVeData data {};
    data.volume = (int16_t)(volume * 0x8000);

    AXVoiceBegin(channel->voice);
    AXSetVoiceVe(channel->voice, &data);
    AXVoiceEnd(channel->voice);
}

float DSP<Console::CAFE>::ChannelGetVolume(size_t id)
{
    AXChannel* channel = &axChannel[id];

    return (float)channel->voice->volume / (float)0x8000;
}

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id)
{
    AXChannel* channel = &axChannel[id];
    AXVoiceOffsets offsets {};

    AXVoiceBegin(channel->voice);
    AXGetVoiceOffsets(channel->voice, &offsets);
    AXVoiceEnd(channel->voice);

    return offsets.currentOffset;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, AXWaveBuf* buffer)
{
    // std::unique_lock lock(this->mutex);
    AXChannel* channel = &axChannel[id];

    if (buffer->state == STATE_QUEUED || buffer->state == STATE_PLAYING)
        return false;

    /* set the offset info up */

    AXVoiceOffsets offsets {};

    std::optional<uint8_t> audioFormat;
    if ((audioFormat = DSP::GetFormat(channel->bitDepth, channel->channels)) < 0)
        return false;

    auto looping = (buffer->looping) ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;

    offsets.dataType       = *audioFormat;
    offsets.loopingEnabled = looping;
    offsets.currentOffset  = 0;
    offsets.data           = buffer->data_pcm16;
    offsets.loopOffset     = 0;
    offsets.endOffset      = buffer->endSamples * channel->channels;

    AXVoiceBegin(channel->voice);
    AXSetVoiceOffsets(channel->voice, &offsets);
    AXVoiceEnd(channel->voice);

    uint16_t current = channel->currentId;
    if (!current)
    {
        current    = 1;
        buffer->id = current;
    }
    else
        buffer->id = current + 1;

    buffer->state = STATE_QUEUED;
    channel->queue.push(buffer);

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceState pausedState = (paused) ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

    PlayState state = (paused) ? STATE_PAUSED : STATE_PLAYING;

    if (channel->state == state)
        return;

    channel->state = state;

    if (channel->queue.size() > 0 && pausedState != AX_VOICE_STATE_PLAYING)
    {
        auto* buffer  = channel->queue.front();
        buffer->state = channel->state;

        AXVoiceBegin(channel->voice);
        AXSetVoiceState(channel->voice, pausedState);
        AXVoiceEnd(channel->voice);
    }
}

bool DSP<Console::CAFE>::IsChannelPaused(size_t id)
{
    AXChannel* channel = &axChannel[id];

    return channel->state == STATE_PAUSED;
}

bool DSP<Console::CAFE>::IsChannelPlaying(size_t id)
{
    AXChannel* channel = &axChannel[id];
    bool running       = false;

    AXVoiceBegin(channel->voice);
    running = AXIsVoiceRunning(channel->voice);
    AXVoiceEnd(channel->voice);

    return running;
}

void DSP<Console::CAFE>::UpdateChannels()
{
    std::unique_lock lock(this->mutex);

    for (size_t index = 0; index < 0x60; index++)
    {
        AXChannel* channel = &axChannel[index];
        auto* buffer       = channel->queue.front();

        if (buffer)
        {
            if (channel->state == STATE_PAUSED)
                continue;

            if (channel->queue.size() > 0)
            {
                if (buffer->state != STATE_PLAYING)
                {
                    AXVoiceBegin(channel->voice);
                    AXSetVoiceState(channel->voice, AX_VOICE_STATE_PLAYING);
                    AXVoiceEnd(channel->voice);

                    buffer->state = STATE_PLAYING;
                }

                if (this->ChannelGetSampleOffset(index) == buffer->endSamples * 2)
                {
                    channel->queue.pop();
                    buffer->state = STATE_FINISHED;
                }
            }
        }
    }
}

void DSP<Console::CAFE>::ChannelStop(size_t id)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceBegin(channel->voice);
    AXSetVoiceState(channel->voice, AX_VOICE_STATE_STOPPED);
    AXVoiceEnd(channel->voice);

    if (channel->voice)
        AXFreeVoice(channel->voice);

    while (auto* buffer = channel->queue.front())
    {
        buffer->state = STATE_FINISHED;
        channel->queue.pop();
    }

    channel->state = STATE_FINISHED;
}

int8_t DSP<Console::CAFE>::GetFormat(int bitDepth, int channels)
{
    /* invalid bitDepth */
    if (bitDepth != 8 && bitDepth != 16)
        return -1;

    /* invalid channel count */
    if (channels < 0 || channels > 2)
        return -2;

    return *DSP::audioFormats.Find(bitDepth);
}
