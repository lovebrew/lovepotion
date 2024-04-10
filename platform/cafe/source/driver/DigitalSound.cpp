#include "common/Exception.hpp"
#include "common/int.hpp"

#include "driver/DigitalSound.hpp"

#include <coreinit/cache.h>
#include <sndcore2/core.h>

namespace love
{
    // clang-format off
    static AXInitParams AX_INIT_PARAMS =
    {
        AX_INIT_RENDERER_48KHZ,
        AX_INIT_PIPELINE_SINGLE,
        0
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

        std::memset(DEVICE_MIX, 0, sizeof(DEVICE_MIX));
    }

    void DigitalSound::updateImpl()
    {
        OSSleepTicks(OSMillisecondsToTicks(5));
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

    AudioBuffer* DigitalSound::createBuffer(int)
    {
        return new AudioBuffer();
    }

    bool DigitalSound::isBufferDone(AudioBuffer* buffer) const
    {
        if (buffer == nullptr)
            return false;

        return buffer->isFinished();
    }

    void DigitalSound::prepareBuffer(AudioBuffer* buffer, size_t nsamples, void* data, size_t size,
                                     bool looping)
    {
        if (buffer == nullptr || data == nullptr)
            return;

        buffer->prepare(nsamples, data, size, looping);
    }

    void DigitalSound::setLooping(AudioBuffer* buffer, bool looping)
    {
        if (buffer == nullptr)
            return;

        buffer->setLooping(looping);
    }

    bool DigitalSound::channelReset(size_t id, AudioBuffer* buffer, int channels, int bitDepth,
                                    int sampleRate)
    {
        if (id >= this->channels.size())
            return false;

        int32_t format = 0;
        if ((format = DigitalSound::getFormat(channels, bitDepth)) < 0)
            return false;

        buffer->initialize(channels, format);
        buffer->setSampleRate(sampleRate);

        return this->channels[id].reset(buffer, channels, 1.0f);
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
