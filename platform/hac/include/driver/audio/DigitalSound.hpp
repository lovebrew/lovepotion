#pragma once

#include "driver/DigitalSound.tcc"

#include <memory>
#include <mutex>
#include <thread>

#include <switch.h>

namespace love
{
    class DigitalSoundMemory;

    using AudioBuffer = AudioDriverWaveBuf;

    class DigitalSound : public DigitalSoundBase<DigitalSound>
    {
      public:
        DigitalSound();

        virtual void initialize() override;

        virtual void deInitialize() override;

        void updateImpl();

        void setMasterVolume(float volume);

        float getMasterVolume() const;

        AudioBuffer createBuffer(int size, int channels);

        void freeBuffer(const AudioBuffer& buffer);

        bool isBufferDone(const AudioBuffer& buffer) const;

        void prepare(AudioBuffer& buffer, const void* data, size_t size, int samples);

        size_t getSampleCount(const AudioBuffer& buffer) const;

        void setLooping(AudioBuffer& buffer, bool looping);

        bool channelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void channelSetVolume(size_t id, float volume);

        float channelGetVolume(size_t id) const;

        size_t channelGetSampleOffset(size_t id);

        bool channelAddBuffer(size_t id, AudioBuffer* buffer);

        void channelPause(size_t id, bool paused);

        bool isChannelPaused(size_t id);

        bool isChannelPlaying(size_t id);

        void channelStop(size_t id);

        void* allocateBuffer(size_t size);
        void freeBuffer(void* buffer);

        // clang-format off
        ENUMMAP_DECLARE(AudioFormats, EncodingFormat, PcmFormat,
            { ENCODING_PCM8,  PcmFormat_Int8  },
            { ENCODING_PCM16, PcmFormat_Int16 }
        );
        // clang-format on

        static int8_t getFormat(int channels, int bitDepth);

      private:
        std::mutex mutex;
        AudioDriver driver;
        bool resetChannel;
        DigitalSoundMemory* memory;
    };
} // namespace love
