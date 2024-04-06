#pragma once

#include "driver/DigitalSound.tcc"

#include <3ds.h>

using NdspEncodingType = decltype(NDSP_ENCODING_PCM8);
using NdspInterpType   = decltype(NDSP_INTERP_LINEAR);

namespace love
{
    using AudioBuffer = ndspWaveBuf;

    class DigitalSound : public DigitalSoundBase<DigitalSound>
    {
      public:
        ~DigitalSound();

        virtual void initialize() override;

        void updateImpl();

        void setMasterVolume(float volume);

        float getMasterVolume() const;

        bool channelReset(size_t id, int channels, int bitDepth, int sampleRate);

        void channelSetVolume(size_t id, float volume);

        float channelGetVolume(size_t id) const;

        size_t channelGetSampleOffset(size_t id) const;

        bool channelAddBuffer(size_t id, AudioBuffer* buffer);

        void channelPause(size_t id, bool paused);

        bool isChannelPaused(size_t id) const;

        bool isChannelPlaying(size_t id) const;

        void channelStop(size_t id);

        // clang-format off
        ENUMMAP_DECLARE(AudioFormats, EncodingFormat, NdspEncodingType,
            { ENCODING_PCM8,  NDSP_ENCODING_PCM8  },
            { ENCODING_PCM16, NDSP_ENCODING_PCM16 }
        );

        ENUMMAP_DECLARE(AudioInterpretedTypes, InterpretedFormat, NdspInterpType,
            { FORMAT_MONO,   NDSP_INTERP_LINEAR    },
            { FORMAT_STEREO, NDSP_INTERP_POLYPHASE }
        );
        // clang-format on

      private:
        static constexpr int32_t DSP_FIRM_MISSING_ERROR_CODE = 0xD880A7FA;

        LightEvent event;

        static uint8_t getNdspFormat(int channels, int bitDepth);
    };
} // namespace love
