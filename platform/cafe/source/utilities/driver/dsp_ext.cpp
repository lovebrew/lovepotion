#include <common/exception.hpp>

#include <utilities/driver/dsp_ext.hpp>
#include <utilities/driver/dsp_mix.hpp>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/drcvs.h>

using namespace love;

// clang-format off
static AXInitParams params = {
    .renderer = AX_INIT_RENDERER_48KHZ,
    .pipeline = AX_INIT_PIPELINE_SINGLE
};
// clang-format on

static void audioCallback()
{
    OSEvent event = DSP<Console::CAFE>::Instance().GetEvent();
    OSSignalEvent(&event);
}

DSP<Console::CAFE>::DSP() : channels {}
{}

void DSP<Console::CAFE>::Initialize()
{
    if (!AXIsInit())
        AXInitWithParams(&params);

    if (!AXIsInit())
        throw love::Exception("Failed to initialize AX!");

    OSInitEvent(&this->event, false, OS_EVENT_MODE_AUTO);
    AXRegisterFrameCallback(audioCallback);

    this->initialized = true;
}

DSP<Console::CAFE>::~DSP()
{
    if (this->initialized)
        AXQuit();
}

void DSP<Console::CAFE>::Update()
{
    OSWaitEvent(&this->event);
}

OSEvent DSP<Console::CAFE>::GetEvent()
{
    return this->event;
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

AXVoice* DSP<Console::CAFE>::FindVoice(size_t channel)
{
    auto iterator = this->channels.find(channel);

    if (iterator == this->channels.end())
        return nullptr;

    return iterator->second;
}

bool DSP<Console::CAFE>::ChannelReset(size_t id, int channels, int bitDepth, int sampleRate)
{
    auto* voice = this->FindVoice(id);

    if (!voice)
        this->channels.insert(std::make_pair(id, new AXVoice()));

    AXVoiceBegin(voice);
    AXSetVoiceType(voice, 0);

    this->ChannelSetVolume(id, this->ChannelGetVolume(id));

    AudioFormat format = (channels == 2) ? FORMAT_STEREO : FORMAT_MONO;

    for (int channel = 0; channel < channels; channel++)
    {
        switch (format)
        {
            case FORMAT_MONO:
            {
                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, MONO_MIX[channel]);
                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, MONO_MIX[channel]);
                break;
            }
            case FORMAT_STEREO:
            {
                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_DRC, 0, STEREO_MIX[channel]);
                AXSetVoiceDeviceMix(voice, AX_DEVICE_TYPE_TV, 0, STEREO_MIX[channel]);
                break;
            }
        }
    }

    float ratio = sampleRate / (float)AXGetInputSamplesPerSec();
    AXSetVoiceSrcRatio(voice, ratio);
    AXSetVoiceSrcType(voice, AX_VOICE_SRC_TYPE_LINEAR);

    AXVoiceOffsets offsets {};

    offsets.dataType = (bitDepth == 8) ? AX_VOICE_FORMAT_LPCM8 : AX_VOICE_FORMAT_LPCM16;
    AXSetVoiceOffsets(voice, &offsets);

    return false;
}

void DSP<Console::CAFE>::ChannelSetVolume(size_t id, float volume)
{
    auto* voice = this->FindVoice(id);

    if (!voice)
        return;

    AXVoiceVeData data {};
    data.volume = volume * 0x8000;

    AXSetVoiceVe(voice, &data);
}

float DSP<Console::CAFE>::ChannelGetVolume(size_t id)
{
    auto* voice = this->FindVoice(id);

    if (!voice)
        return;

    return voice->volume;
}

size_t DSP<Console::CAFE>::ChannelGetSampleOffset(size_t id)
{
    auto* voice = this->FindVoice(id);

    if (!voice)
        return;

    AXVoiceOffsets offsets {};
    AXGetVoiceOffsets(voice, &offsets);

    return offsets.currentOffset;
}

bool DSP<Console::CAFE>::ChannelAddBuffer(size_t id)
{
    auto* voice = this->FindVoice(id);

    if (voice)
        return false;

    AXSetVoiceState(voice, AX_VOICE_STATE_PLAYING);

    return true;
}

void DSP<Console::CAFE>::ChannelPause(size_t id, bool paused)
{
    AXVoice* voice = this->FindVoice(id);

    if (!voice)
        return;

    AXVoiceState pausedState = (paused) ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

    AXSetVoiceState(voice, pausedState);
}

bool DSP<Console::CAFE>::IsChannelPaused(size_t id)
{
    AXVoice* voice = this->FindVoice(id);

    if (!voice)
        return;

    return voice->state == AX_VOICE_STATE_STOPPED;
}

bool DSP<Console::CAFE>::IsChannelPlaying(size_t id)
{
    AXVoice* voice = this->FindVoice(id);

    if (!voice)
        return;

    return voice->state == AX_VOICE_STATE_PLAYING;
}

void DSP<Console::CAFE>::ChannelStop(size_t id)
{
    AXVoice* voice = this->FindVoice(id);

    if (!voice)
        return;

    AXVoiceEnd(voice);
    AXFreeVoice(voice);
    this->channels.erase(id);
}
