#include "common/Exception.hpp"
#include "common/config.hpp"
#include "common/int.hpp"

#include "driver/audio/DigitalSound.hpp"

#include <coreinit/cache.h>
#include <sndcore2/core.h>

#include <cstring>

namespace love
{
    // clang-format off
    static AXInitParams AX_INIT_PARAMS =
    {
        .renderer = AX_INIT_RENDERER_48KHZ,
        .pipeline = AX_INIT_PIPELINE_SINGLE
    };
    // clang-format on

    void DigitalSound::initialize()
    {
        if (!AXIsInit())
            AXInitWithParams(&AX_INIT_PARAMS);

        if (!AXIsInit())
            throw love::Exception("Failed to initialize AX.");
    }

    void DigitalSound::deInitialize()
    {
        AXQuit();
    }

    void DigitalSound::updateImpl()
    {
        for (auto& channel : this->channels)
            channel.update();

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

    AudioBuffer DigitalSound::createBuffer(int size, int channels)
    {
        AudioBuffer buffer {};

        for (int channel = 0; channel < channels; channel++)
        {
            buffer.data_pcm16[channel] = (int16_t*)malloc(size);

            if (buffer.data_pcm16[channel] == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
        }

        buffer.setChannelCount(channels);

        return buffer;
    }

    void DigitalSound::freeBuffer(const AudioBuffer& buffer)
    {
        if (buffer.data_pcm16[0])
            free(buffer.data_pcm16[0]);

        if (buffer.data_pcm16[1])
            free(buffer.data_pcm16[1]);
    }

    bool DigitalSound::isBufferDone(const AudioBuffer& buffer) const
    {
        return buffer.isFinished();
    }

    void DigitalSound::prepare(AudioBuffer& buffer, void* data, size_t size, int samples)
    {
        if (data == nullptr)
            return;

        buffer.nsamples = samples;

        switch (buffer.getChannelCount())
        {
            case 1:
                std::memcpy(buffer.data_pcm16[0], data, size);
                DCFlushRange(buffer.data_pcm16[0], size);

                break;
            case 2:
            {
                for (size_t index = 0; index < size; index += 2)
                {
                    buffer.data_pcm16[0][index / 2] = ((int16_t*)data)[index];
                    buffer.data_pcm16[1][index / 2] = ((int16_t*)data)[index + 1];
                }

                DCFlushRange(buffer.data_pcm16[0], size);
                DCFlushRange(buffer.data_pcm16[1], size);

                break;
            }
            default:
                throw love::Exception("Unsupported channel count");
        }
    }

    size_t DigitalSound::getSampleCount(const AudioBuffer& buffer) const
    {
        return buffer.getSampleCount();
    }

    void DigitalSound::setLooping(AudioBuffer& buffer, bool looping)
    {
        buffer.setLooping(looping);
    }

    bool DigitalSound::channelReset(size_t id, int channels, int bitDepth, int sampleRate)
    {
        if (id >= this->channels.size())
            return false;

        this->channels[id].reset();

        int32_t format = 0;
        if ((format = DigitalSound::getFormat(channels, bitDepth)) < 0)
            return false;

        this->channels[id].setFormat(format);
        this->channels[id].setSampleRate((float)sampleRate);
        this->channels[id].setInterpType(channels);

        this->channels[id].setVolume(this->channels[id].getVolume());
        return true;
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
        return this->channels[id].getSamplePosition();
    }

    bool DigitalSound::channelAddBuffer(size_t id, AudioBuffer* buffer)
    {
        return this->channels[id].addWaveBuffer(buffer);
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
