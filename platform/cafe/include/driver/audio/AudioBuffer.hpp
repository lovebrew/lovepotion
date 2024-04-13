#pragma once

#include "common/Exception.hpp"
#include <coreinit/cache.h>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include <algorithm>
#include <array>

namespace love
{
    class AudioBuffer
    {
      public:
        AudioBuffer();

        ~AudioBuffer();

        void initialize(int channels);

        bool isInitialized() const
        {
            return this->ready;
        }

        bool prepare();

        size_t getSampleCount() const
        {
            return this->nsamples;
        }

        void setDeviceMix(int channels);

        void setState(AXVoiceState state);

        bool setFormat(AXVoiceFormat format);

        bool isFinished() const;

        void setVolume(float volume);

        bool setSampleRateConversionRatio(float ratio);

        size_t getSampleOffset() const;

        void setLooping(bool looping);

        bool isLooping() const;

      public:
        int16_t* data;
        size_t nsamples;

      private:
        static AXVoiceLoop getLooping(bool looping)
        {
            return looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
        }

        std::array<AXVoice*, 2> voices;
        int channels;

        bool ready;
    };
} // namespace love
