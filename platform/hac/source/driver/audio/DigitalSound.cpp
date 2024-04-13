#include "common/Exception.hpp"
#include "common/Result.hpp"

#include "driver/audio/DigitalSound.hpp"
#include "driver/audio/DigitalSoundMem.hpp"

#include <cstring>

namespace love
{
    static constexpr AudioRendererConfig config = {
        .output_rate     = AudioRendererOutputRate_48kHz,
        .num_voices      = 24,
        .num_effects     = 0,
        .num_sinks       = 1,
        .num_mix_objs    = 1,
        .num_mix_buffers = 2,
    };

    static constexpr uint8_t sinkChannels[2] = { 0, 1 };

    void DigitalSound::initialize()
    {
        if (bool result = DigitalSoundMemory::getInstance().initialize(); !result)
            throw love::Exception("Failed to create audio memory pool!");

        if (auto result = Result(audrenInitialize(&config)); !result)
            throw love::Exception("Failed to initialize audren: {:x}", result.get());

        if (auto result = Result(audrvCreate(&this->driver, &config, 2)); !result)
            throw love::Exception("Failed to create audio driver: {:x}", result.get());

        // clang-format off
        const auto size = DigitalSoundMemory::getInstance().getSize();
        int poolId = audrvMemPoolAdd(&this->driver, DigitalSoundMemory::getInstance().getBaseAddress(), size);
        // clang-format on

        if (poolId == -1)
            throw love::Exception("Failed to add memory pool!");

        bool attached = audrvMemPoolAttach(&this->driver, poolId);

        if (!attached)
            throw love::Exception("Failed to attach memory pool!");

        int sinkId = audrvDeviceSinkAdd(&this->driver, AUDREN_DEFAULT_DEVICE_NAME, 2, sinkChannels);

        if (sinkId == -1)
            throw love::Exception("Failed to add sink to driver!");

        if (auto result = Result(audrvUpdate(&this->driver)); !result)
            throw love::Exception("Failed to update audio driver: {:x}", result.get());

        if (auto result = Result(audrenStartAudioRenderer()); !result)
            throw love::Exception("Failed to start audio renderer: {:x}", result.get());
    }

    DigitalSound::~DigitalSound()
    {
        audrvClose(&this->driver);
        audrenExit();
    }

    void DigitalSound::updateImpl()
    {
        {
            std::unique_lock lock(this->mutex);
            audrvUpdate(&this->driver);
        }

        audrenWaitFrame();
    }

    void DigitalSound::setMasterVolume(float volume)
    {
        std::unique_lock lock(this->mutex);

        for (int mix = 0; mix < 2; mix++)
            audrvMixSetVolume(&this->driver, mix, volume);
    }

    float DigitalSound::getMasterVolume() const
    {
        return this->driver.in_mixes[0].volume;
    }

    AudioBuffer DigitalSound::createBuffer(int size)
    {
        AudioBuffer buffer {};

        if (size != 0)
        {
            buffer.data_pcm16 = (int16_t*)DigitalSoundMemory::getInstance().allocate(size);

            if (buffer.data_pcm16 == nullptr)
                throw love::Exception(E_OUT_OF_MEMORY);
        }
        else
            throw love::Exception("Size cannot be zero.");

        return buffer;
    }

    void DigitalSound::freeBuffer(const AudioBuffer& buffer)
    {
        DigitalSoundMemory::getInstance().free(buffer.data_pcm16);
    }

    bool DigitalSound::isBufferDone(const AudioBuffer& buffer) const
    {
        return buffer.state == AudioDriverWaveBufState_Done;
    }

    void DigitalSound::prepare(AudioBuffer& buffer, const void* data, size_t size, int samples)
    {
        if (data == nullptr)
            return;

        buffer.size              = size;
        buffer.end_sample_offset = samples;

        std::memcpy(buffer.data_pcm16, data, size);

        armDCacheFlush(buffer.data_pcm16, size);
    }

    size_t DigitalSound::getSampleCount(const AudioBuffer& buffer) const
    {
        return buffer.end_sample_offset;
    }

    void DigitalSound::setLooping(AudioBuffer& buffer, bool looping)
    {
        buffer.is_looping = looping;
    }

    bool DigitalSound::channelReset(size_t id, int channels, int bitDepth, int sampleRate)
    {
        std::unique_lock lock(this->mutex);

        int8_t format = PcmFormat_Invalid;
        if ((format = DigitalSound::getFormat(channels, bitDepth)) < 0)
            return false;

        this->resetChannel =
            audrvVoiceInit(&this->driver, id, channels, (PcmFormat)format, sampleRate);

        if (this->resetChannel)
        {
            audrvVoiceSetDestinationMix(&this->driver, id, AUDREN_FINAL_MIX_ID);
            audrvVoiceSetMixFactor(&this->driver, id, 1.0f, 0, 0);

            if (channels == 2)
                audrvVoiceSetMixFactor(&this->driver, id, 1.0f, 0, 1);
        }

        return this->resetChannel;
    }

    void DigitalSound::channelSetVolume(size_t id, float volume)
    {
        std::unique_lock lock(this->mutex);

        audrvVoiceSetVolume(&this->driver, id, volume);
    }

    float DigitalSound::channelGetVolume(size_t id) const
    {
        return this->driver.in_voices[id].volume;
    }

    size_t DigitalSound::channelGetSampleOffset(size_t id)
    {
        std::unique_lock lock(this->mutex);

        return audrvVoiceGetPlayedSampleCount(&this->driver, id);
    }

    bool DigitalSound::channelAddBuffer(size_t id, AudioBuffer* buffer)
    {
        if (this->resetChannel)
        {
            std::unique_lock lock(this->mutex);

            bool success = audrvVoiceAddWaveBuf(&this->driver, id, buffer);

            if (success)
                audrvVoiceStart(&this->driver, id);

            return success;
        }

        return false;
    }

    void DigitalSound::channelPause(size_t id, bool paused)
    {
        std::unique_lock lock(this->mutex);

        audrvVoiceSetPaused(&this->driver, id, paused);
    }

    bool DigitalSound::isChannelPaused(size_t id)
    {
        return audrvVoiceIsPaused(&this->driver, id);
    }

    bool DigitalSound::isChannelPlaying(size_t id)
    {
        return audrvVoiceIsPlaying(&this->driver, id);
    }

    void DigitalSound::channelStop(size_t id)
    {
        std::unique_lock lock(this->mutex);

        audrvVoiceStop(&this->driver, id);
    }

    int8_t DigitalSound::getFormat(int channels, int bitDepth)
    {
        if (bitDepth != 8 && bitDepth != 16)
            return -1;

        if (channels < 1 || channels > 2)
            return -2;

        PcmFormat format;
        DigitalSound::getConstant((EncodingFormat)bitDepth, format);

        return format;
    }
} // namespace love
