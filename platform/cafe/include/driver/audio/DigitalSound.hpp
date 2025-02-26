#pragma once

#include "driver/DigitalSound.tcc"
#include "driver/audio/AudioBuffer.hpp"
#include "driver/audio/SoundChannel.hpp"

#include <coreinit/event.h>

extern "C"
{
    void AXSetMasterVolume(uint32_t volume);
    uint16_t AXGetMasterVolume();
}

namespace love
{
    class DigitalSound : public DigitalSoundBase<DigitalSound>
    {
      public:
        virtual void initialize() override;

        virtual void deInitialize() override;

        void updateImpl();

        void setMasterVolume(float volume);

        float getMasterVolume() const;

        AudioBuffer createBuffer(int size, int channels);

        void freeBuffer(const AudioBuffer& buffer);

        bool isBufferDone(const AudioBuffer& buffer) const;

        void prepare(AudioBuffer& buffer, void* data, size_t size, int samples);

        size_t getSampleCount(const AudioBuffer& buffer) const;

        void setLooping(AudioBuffer& buffer, bool looping);

        bool channelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void channelSetVolume(size_t id, float volume);

        float channelGetVolume(size_t id) const;

        size_t channelGetSampleOffset(size_t id);

        bool channelAddBuffer(size_t id, AudioBuffer* buffer);

        void channelPause(size_t id, bool paused);

        bool isChannelPaused(size_t id) const;

        bool isChannelPlaying(size_t id) const;

        void channelStop(size_t id);

        static int32_t getFormat(int channels, int bitDepth);

        OSEvent& getEvent()
        {
            return this->event;
        }

        bool isFinishing() const
        {
            return this->finish;
        }

        // clang-format off
        ENUMMAP_DECLARE(AudioFormats, EncodingFormat, AX_VOICE_FORMAT,
            { ENCODING_PCM8,  AX_VOICE_FORMAT_LPCM8  },
            { ENCODING_PCM16, AX_VOICE_FORMAT_LPCM16 }
        );
        // clang-format on

      private:
        std::array<SoundChannel, 24> channels;
        OSEvent event;
        bool finish;
    };
} // namespace love
