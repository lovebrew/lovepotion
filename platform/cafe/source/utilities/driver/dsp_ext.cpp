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
    DSP<Console::CAFE>::Instance().UpdateChannels();
    DSP<Console::CAFE>::Instance().SignalEvent();
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
    AXRegisterAppFrameCallback(audioCallback);

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

static void resetVoice(AXVoice* voice, int channels, int sampleRate)
{
    AXVoiceBegin(voice);
    AXSetVoiceType(voice, 0);

    AXVoiceVeData data {};
    data.volume = 0x8000;

    AXSetVoiceVe(voice, &data);

    switch (channels)
    {
        case 1:
        {
            AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, MONO_MIX[0]);
            AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, MONO_MIX[0]);
            break;
        }
        case 2:
        default:
            AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, STEREO_MIX);
            AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, STEREO_MIX);
    }

    float ratio = (sampleRate / (float)AXGetInputSamplesPerSec()) * channels;

    AXSetVoiceSrcRatio(voice, ratio);
    AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

    AXVoiceEnd(voice);
}

bool DSP<Console::CAFE>::ChannelReset(size_t id, int channels, int bitDepth, int sampleRate)
{
    AXChannel* channel = &axChannel[id];

    channel->channels   = channels;
    channel->bitDepth   = bitDepth;
    channel->sampleRate = sampleRate;

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

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, AXVoice* buffer)
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

    auto looping =
        (buffer->offsets.loopingEnabled) ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;

    offsets.dataType       = *audioFormat;
    offsets.loopingEnabled = looping;
    offsets.currentOffset  = 0;
    offsets.data           = buffer->offsets.data;
    offsets.loopOffset     = 0;
    offsets.endOffset      = buffer->offsets.endOffset * channel->channels;

    buffer->state = STATE_QUEUED;

    if (channel->queue.size() > 0)
    {
        AXVoice* voice = AXAcquireVoice(31, nullptr, nullptr);
        resetVoice(voice, channel->channels, channel->sampleRate);

        AXVoiceBegin(voice);
        AXSetVoiceOffsets(voice, &offsets);
        AXVoiceEnd(voice);

        channel->queue.push(std::make_pair(voice, offsets.endOffset));

        return true;
    }

    if (!channel->voice)
        channel->voice = AXAcquireVoice(31, nullptr, nullptr);

    AXVoiceBegin(channel->voice);
    AXSetVoiceOffsets(channel->voice, &offsets);
    AXVoiceEnd(channel->voice);

    channel->queue.push(std::make_pair(channel->voice, offsets.endOffset));

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceState pausedState = (paused) ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

    PlayState state = (paused) ? STATE_PAUSED : STATE_PLAYING;

    channel->state = state;

    AXVoiceBegin(channel->voice);
    AXSetVoiceState(channel->voice, pausedState);
    AXVoiceEnd(channel->voice);
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

        if (channel->state == STATE_PAUSED)
            continue;

        if (channel->queue.size() > 0)
        {
            const auto& info = channel->queue.front();

            while (channel->queue.size() > 0)
            {
                if (info.first->state != AX_VOICE_STATE_PLAYING)
                {
                    AXVoiceBegin(info.first);
                    AXSetVoiceState(info.first, AX_VOICE_STATE_PLAYING);
                    AXVoiceEnd(info.first);
                    break;
                }

                if ((uint32_t)this->ChannelGetSampleOffset(index) == info.second)
                {
                    channel->voice->state = STATE_FINISHED;
                    channel->queue.pop();

                    channel->voice = channel->queue.front().first;
                }
            }

            if ((uint32_t)this->ChannelGetSampleOffset(index) == info.second)
            {
                channel->voice->state = STATE_FINISHED;
                channel->queue.pop();

                channel->voice = channel->queue.front().first;
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

    for (size_t index = 0; index < channel->queue.size(); index++)
    {
        channel->queue.front().first->state = STATE_FINISHED;
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
