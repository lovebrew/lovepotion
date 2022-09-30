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
    .pipeline = AX_INIT_PIPELINE_SINGLE
};
// clang-format on

static void audioCallback()
{
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
    auto result = AXRegisterAppFrameCallback(audioCallback);
    LOG("AXRegisterAppFrameCallback: %d", result);

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

    channel->channels  = channels;
    AudioFormat format = (channels == 2) ? FORMAT_STEREO : FORMAT_MONO;

    for (int mixId = 0; mixId < channels; mixId++)
    {
        if (!channel->voices[mixId])
            channel->voices[mixId] = AXAcquireVoice(31, nullptr, nullptr);

        AXVoiceBegin(channel->voices[mixId]);

        AXSetVoiceType(channel->voices[mixId], AX_VOICE_TYPE_UNKNOWN);

        AXVoiceVeData data {};
        data.volume = 0x8000;

        AXSetVoiceVe(channel->voices[mixId], &data);

        if (format == FORMAT_MONO)
        {
            AXSetVoiceDeviceMix(channel->voices[mixId], AX_DEVICE_TYPE_DRC, 0, MONO_MIX[mixId]);
            AXSetVoiceDeviceMix(channel->voices[mixId], AX_DEVICE_TYPE_TV, 0, MONO_MIX[mixId]);
        }
        else if (format == FORMAT_STEREO)
        {
            AXSetVoiceDeviceMix(channel->voices[mixId], AX_DEVICE_TYPE_DRC, 0, STEREO_MIX[mixId]);
            AXSetVoiceDeviceMix(channel->voices[mixId], AX_DEVICE_TYPE_TV, 0, STEREO_MIX[mixId]);
        }

        float ratio = sampleRate / (float)AXGetInputSamplesPerSec();
        AXSetVoiceSrcRatio(channel->voices[mixId], ratio);
        AXSetVoiceSrcType(channel->voices[mixId], AX_VOICE_SRC_TYPE_LINEAR);

        AXVoiceEnd(channel->voices[mixId]);
    }

    return true;
}

void DSP<Console::CAFE>::ChannelSetVolume(size_t id, float volume)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceVeData data {};
    data.volume = (int16_t)(volume * 0x8000);

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        AXVoiceBegin(channel->voices[mixId]);
        AXSetVoiceVe(channel->voices[mixId], &data);
        AXVoiceEnd(channel->voices[mixId]);
    }
}

float DSP<Console::CAFE>::ChannelGetVolume(size_t id)
{
    AXChannel* channel = &axChannel[id];

    return (float)channel->voices[0]->volume / (float)0x8000;
}

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id)
{
    AXChannel* channel = &axChannel[id];
    AXVoiceOffsets offsets {};

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        AXVoiceBegin(channel->voices[mixId]);
        AXGetVoiceOffsets(channel->voices[mixId], &offsets);
        AXVoiceEnd(channel->voices[mixId]);
    }

    return offsets.currentOffset;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id, AXWaveBuf* buffer)
{
    AXChannel* channel = &axChannel[id];

    if (buffer->state == STATE_QUEUED || buffer->state == STATE_PLAYING)
        return false;

    buffer->next   = nullptr;
    buffer->state  = STATE_QUEUED;
    channel->state = STATE_QUEUED;

    auto* callback = channel->buffer;

    if (callback)
    {
        while (callback->next)
            callback = callback->next;

        callback->next = buffer;
    }
    else
        channel->buffer = buffer;

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        AXVoiceOffsets offsets {};

        offsets.dataType = (buffer->bitDepth == 8) ? AX_VOICE_FORMAT_LPCM8 : AX_VOICE_FORMAT_LPCM16;
        offsets.loopingEnabled = (buffer->looping) ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
        offsets.currentOffset  = 0;
        offsets.data           = buffer->data_pcm16;
        offsets.loopOffset     = 0;

        AXVoiceBegin(channel->voices[mixId]);
        AXSetVoiceOffsets(channel->voices[mixId], &offsets);
        AXSetVoiceEndOffset(channel->voices[mixId], buffer->endSamples);
        AXSetVoiceState(channel->voices[mixId], AX_VOICE_STATE_PLAYING);
        AXVoiceEnd(channel->voices[mixId]);
    }

    buffer->state  = STATE_PLAYING;
    channel->state = STATE_PLAYING;

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    AXChannel* channel = &axChannel[id];

    AXVoiceState pausedState = (paused) ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

    channel->state         = (paused) ? STATE_PAUSED : STATE_PLAYING;
    channel->buffer->state = channel->state;

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        AXVoiceBegin(channel->voices[mixId]);
        AXSetVoiceState(channel->voices[mixId], pausedState);
        AXVoiceEnd(channel->voices[mixId]);
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

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        AXVoiceBegin(channel->voices[mixId]);
        running = AXIsVoiceRunning(channel->voices[mixId]);
        AXVoiceEnd(channel->voices[mixId]);
    }

    return running;
}

void DSP<Console::CAFE>::CheckChannelFinished(size_t id)
{
    if (this->IsChannelPlaying(id))
        return;

    {
        AXChannel* channel = &axChannel[id];

        channel->state         = STATE_FINISHED;
        channel->buffer->state = STATE_FINISHED;

        for (int mixId = 0; mixId < channel->channels; mixId++)
            channel->voices[mixId]->state = STATE_FINISHED;
    }
}

void DSP<Console::CAFE>::ChannelStop(size_t id)
{
    AXChannel* channel = &axChannel[id];

    for (int mixId = 0; mixId < channel->channels; mixId++)
    {
        if (channel->voices[mixId])
            AXFreeVoice(channel->voices[mixId]);
    }

    if (channel->buffer)
        channel->buffer->state = STATE_FINISHED;

    channel->state = STATE_FINISHED;
}
