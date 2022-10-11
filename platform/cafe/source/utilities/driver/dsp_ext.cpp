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

static void dspResetVoice(AXVoice* voice, int channels, int sampleRate)
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

    channel->channels        = channels;
    channel->bitDepth        = bitDepth;
    channel->sampleRate      = sampleRate;
    channel->buffersConsumed = 0;

    for (auto* buffer = channel->firstWaveBuf; buffer; buffer = buffer->cbNext)
    {
        AXVoiceBegin(buffer);
        AXSetVoiceState(buffer, STATE_FINISHED);
        AXVoiceEnd(buffer);
    }

    channel->firstWaveBuf   = nullptr;
    channel->lastWaveBuf    = nullptr;
    channel->waitingWaveBuf = nullptr;

    return true;
}

void DSP<Console::CAFE>::ChannelSetVolume(size_t id, float volume)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceVeData data {};
    data.volume = (int16_t)(volume * 0x8000);

    AXVoiceBegin(channel->firstWaveBuf);
    AXSetVoiceVe(channel->firstWaveBuf, &data);
    AXVoiceEnd(channel->firstWaveBuf);
}

float DSP<Console::CAFE>::ChannelGetVolume(size_t id)
{
    AXChannel* channel = &axChannel[id];

    return (float)channel->firstWaveBuf->volume / (float)0x8000;
}

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id)
{
    AXChannel* channel = &axChannel[id];
    AXVoiceOffsets offsets {};

    if (!channel->firstWaveBuf)
        return 0;

    AXVoiceBegin(channel->firstWaveBuf);
    AXGetVoiceOffsets(channel->firstWaveBuf, &offsets);
    AXVoiceEnd(channel->firstWaveBuf);

    return offsets.currentOffset;
}

static void dspQueueBufer(AXChannel* channel)
{
    AXVoice* voice = channel->waitingWaveBuf;

    while (voice && channel->bufferCount < 4)
    {
        voice->state = DSP<Console::CAFE>::STATE_QUEUED;

        AXVoiceOffsets offsets {};

        std::optional<uint8_t> audioFormat;
        if ((audioFormat = DSP<Console::CAFE>::GetFormat(channel->bitDepth, channel->channels)) < 0)
            return;

        offsets.dataType       = *audioFormat;
        offsets.loopingEnabled = (AXVoiceLoop)voice->offsets.loopingEnabled;
        offsets.currentOffset  = 0;
        offsets.data           = voice->offsets.data;
        offsets.loopOffset     = 0;
        offsets.endOffset      = voice->offsets.endOffset * channel->channels;

        dspResetVoice(voice, channel->channels, channel->sampleRate);

        AXVoiceBegin(voice);
        AXSetVoiceOffsets(voice, &offsets);
        AXVoiceEnd(voice);

        channel->bufferCount++;
        voice = voice->cbNext;
    }

    channel->waitingWaveBuf = voice;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, AXVoice* buffer)
{
    // std::unique_lock lock(this->mutex);
    AXChannel* channel = &axChannel[id];

    if (buffer->state == STATE_QUEUED || buffer->state == STATE_PLAYING)
        return false;

    buffer->cbNext = nullptr;

    if (channel->firstWaveBuf)
        channel->lastWaveBuf->cbNext = buffer;
    else
        channel->firstWaveBuf = buffer;

    if (!channel->waitingWaveBuf)
        channel->waitingWaveBuf = buffer;

    channel->lastWaveBuf = buffer;

    dspQueueBufer(channel);

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceState pausedState = (paused) ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

    PlayState state = (paused) ? STATE_PAUSED : STATE_STARTED;

    channel->state = state;

    if (!channel->firstWaveBuf || state == STATE_STARTED)
        return;

    AXVoiceBegin(channel->firstWaveBuf);
    AXSetVoiceState(channel->firstWaveBuf, pausedState);
    AXVoiceEnd(channel->firstWaveBuf);
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

    if (!channel->firstWaveBuf)
        return false;

    AXVoiceBegin(channel->firstWaveBuf);
    running = AXIsVoiceRunning(channel->firstWaveBuf);
    AXVoiceEnd(channel->firstWaveBuf);

    return running;
}

void DSP<Console::CAFE>::UpdateChannels()
{
    for (size_t channelId = 0; channelId < 0x60; channelId++)
    {
        AXChannel* channel = &axChannel[channelId];

        AXVoice* voice = channel->firstWaveBuf;

        for (size_t index = 0; voice && index < channel->bufferCount; index++)
        {
            if (this->ChannelGetSampleOffset(channelId) == voice->offsets.endOffset)
            {
                AXVoiceBegin(voice);
                AXSetVoiceState(voice, STATE_FINISHED);
                AXVoiceEnd(voice);

                voice = voice->cbNext;
                channel->bufferCount--;
            }
        }

        if (voice)
        {
            AXVoiceBegin(voice);
            AXSetVoiceState(voice, STATE_PLAYING);
            AXVoiceEnd(voice);

            dspQueueBufer(channel);
            channel->firstWaveBuf = voice;
        }
        else
        {
            channel->firstWaveBuf = nullptr;
            channel->lastWaveBuf  = nullptr;
        }
    }
}

void DSP<Console::CAFE>::ChannelStop(size_t id)
{
    AXChannel* channel = &axChannel[id];

    if (!channel->firstWaveBuf)
        return;

    AXVoiceBegin(channel->firstWaveBuf);
    AXSetVoiceState(channel->firstWaveBuf, AX_VOICE_STATE_STOPPED);
    AXVoiceEnd(channel->firstWaveBuf);

    AXFreeVoice(channel->firstWaveBuf);

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
