#pragma once

#include "driver/DigitalSound.tcc"
#include "driver/DigitalSoundMix.hpp"
#include "driver/SoundChannel.hpp"

#include <coreinit/event.h>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

extern "C"
{
    void AXSetMasterVolume(uint32_t volume);
    uint16_t AXGetMasterVolume();
}

namespace love
{
    using AudioBuffer = AXVoice;

    class DigitalSound : public DigitalSoundBase<DigitalSound>
    {

      public:
        ~DigitalSound();

        virtual void initialize() override;

        void updateImpl();

        void setMasterVolume(float volume);

        float getMasterVolume() const;

        AudioBuffer* createBuffer(int size = 0);

        bool isBufferDone(AudioBuffer* buffer) const;

        void prepareBuffer(AudioBuffer* buffer, size_t nsamples, void* data, size_t size,
                           bool looping);

        void setLooping(AudioBuffer* buffer, bool looping);

        bool channelReset(size_t id, AudioBuffer* buffer, int channels, int bitDepth,
                          int sampleRate);

        void channelSetVolume(size_t id, float volume);

        float channelGetVolume(size_t id) const;

        size_t channelGetSampleOffset(size_t id);

        bool channelAddBuffer(size_t id, AudioBuffer* buffer);

        void channelPause(size_t id, bool paused);

        bool isChannelPaused(size_t id) const;

        bool isChannelPlaying(size_t id) const;

        void channelStop(size_t id);

        static int32_t getFormat(int channels, int bitDepth);

        // clang-format off
        ENUMMAP_DECLARE(AudioFormats, EncodingFormat, AX_VOICE_FORMAT,
            { ENCODING_PCM8,  AX_VOICE_FORMAT_LPCM8  },
            { ENCODING_PCM16, AX_VOICE_FORMAT_LPCM16 }
        );
        // clang-format on

        OSEvent& getEvent()
        {
            return event;
        }

      private:
        OSEvent event;

        std::array<SoundChannel, 24> channels;
        bool shutdown = false;
    };
} // namespace love
