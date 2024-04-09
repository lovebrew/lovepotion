#include "common/Exception.hpp"
#include "common/int.hpp"

#include "driver/DigitalSound.hpp"

#include <coreinit/cache.h>
#include <sndcore2/core.h>

namespace love
{
    void audioCallback()
    {}

    // clang-format off
    static AXInitParams AX_INIT_PARAMS = {
        .renderer = AX_INIT_RENDERER_48KHZ,
        .pipeline = AX_INIT_PIPELINE_SINGLE
    };
    // clang-format on

    DigitalSound::~DigitalSound()
    {
        AXQuit();
    }

    void DigitalSound::initialize()
    {
        if (!AXIsInit())
            AXInitWithParams(&AX_INIT_PARAMS);

        if (!AXIsInit())
            throw love::Exception("Failed to initialize AX.");

        // OSInitEvent(&this->event, false, OS_EVENT_MODE_AUTO);
        // AXRegisterAppFrameCallback(audioCallback);
    }

    void DigitalSound::updateImpl()
    {
        OSSleepTicks(OSMillisecondsToTicks(3));
    }

    void DigitalSound::setMasterVolume(float volume)
    {
        AXSetMasterVolume(volume * 0x8000);
    }

    float DigitalSound::getMasterVolume() const
    {
        uint16_t volume = AXGetMasterVolume();
        return volume / (float)0x8000;
    }

    AudioBuffer* DigitalSound::createBuffer(int size)
    {
        AXVoice* voice = new AXVoice();

        AXVoiceBegin(voice);

        AXSetVoiceState(voice, AX_VOICE_STATE_STOPPED);

        AXVoiceEnd(voice);

        return voice;
    }

    bool DigitalSound::isBufferDone(AudioBuffer* buffer) const
    {
        if (buffer == nullptr)
            return false;

        bool done = false;

        AXVoiceBegin(buffer);
        done = buffer->state == AX_VOICE_STATE_STOPPED;
        AXVoiceEnd(buffer);

        return done;
    }

    void DigitalSound::prepareBuffer(AudioBuffer* buffer, size_t nsamples, void* data, size_t size,
                                     bool looping)
    {
        if (buffer == nullptr || data == nullptr)
            return;

        AXVoiceBegin(buffer);

        DCStoreRange(data, size);

        AXSetVoiceState(buffer, AX_VOICE_STATE_STOPPED);
        AXSetVoiceLoop(buffer, looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED);

        AXVoiceOffsets offsets {};
        AXGetVoiceOffsets(buffer, &offsets);

        offsets.data          = (int16_t*)data;
        offsets.currentOffset = 0;
        offsets.endOffset     = nsamples;

        AXSetVoiceOffsets(buffer, &offsets);

        AXVoiceEnd(buffer);
    }

    void DigitalSound::setLooping(AudioBuffer* buffer, bool looping)
    {
        if (buffer == nullptr)
            return;

        AXVoiceBegin(buffer);

        AXSetVoiceLoop(buffer, looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED);

        AXVoiceEnd(buffer);
    }

    bool DigitalSound::channelReset(size_t id, AudioBuffer* buffer, int channels, int bitDepth,
                                    int sampleRate)
    {
        if (id >= this->channels.size())
            return false;

        this->channels[id].begin(buffer, channels);
        return this->channels[id].reset(channels, bitDepth, sampleRate, 1.0f);
    }

    void DigitalSound::channelSetVolume(size_t id, float volume)
    {
        this->channels[id].setVolume(volume);
    }

    float DigitalSound::channelGetVolume(size_t id) const
    {
        return this->channels[id].getVolume();
    }

    size_t DigitalSound::channelGetSampleOffset(size_t id)
    {
        return this->channels[id].getSampleOffset();
    }

    bool DigitalSound::channelAddBuffer(size_t id, AudioBuffer*)
    {
        return this->channels[id].play();
    }

    void DigitalSound::channelStop(size_t id)
    {
        this->channels[id].stop();
    }

    void DigitalSound::channelPause(size_t id, bool paused)
    {
        this->channels[id].setPaused(paused);
    }

    bool DigitalSound::isChannelPaused(size_t id) const
    {
        return this->channels[id].isPaused();
    }

    bool DigitalSound::isChannelPlaying(size_t id) const
    {
        return this->channels[id].isPlaying();
    }

    int32_t DigitalSound::getFormat(int channels, int bitDepth)
    {
        if (bitDepth != 8 && bitDepth != 16)
            return -1;

        if (channels < 1 || channels > 2)
            return -2;

        AX_VOICE_FORMAT format;
        DigitalSound::getConstant((EncodingFormat)bitDepth, format);

        return format;
    }

} // namespace love
