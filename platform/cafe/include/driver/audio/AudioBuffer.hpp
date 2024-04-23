#pragma once

#include "common/Exception.hpp"
#include <coreinit/cache.h>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include <algorithm>

#include <array>
#include <vector>

namespace love
{
    class AudioBuffer
    {
      public:
        AudioBuffer();

        AudioBuffer(const AudioBuffer&) = delete;

        AudioBuffer(AudioBuffer&& other)
        {
            this->data_pcm16 = std::move(other.data_pcm16);
            this->nsamples   = other.nsamples;
            this->voices     = std::move(other.voices);
            this->channels   = other.channels;
            this->ready      = other.ready;
            this->playing    = other.playing;

            other.ready   = false;
            other.playing = false;
        }

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

        bool isPlaying() const;

        bool isFinished() const;

        void setVolume(float volume);

        bool setSampleRateConversionRatio(float ratio);

        size_t getSampleOffset() const;

        void setLooping(bool looping);

        bool isLooping() const;

        int getChannelCount() const
        {
            return this->channels;
        }

        void setChannelCount(int channels)
        {
            this->channels = channels;
        }

        void setPlaying(bool playing)
        {
            this->playing = playing;
        }

      public:
        std::array<int16_t*, 2> data_pcm16;
        size_t nsamples;
        uint32_t offset;

      private:
        static AXVoiceLoop getLooping(bool looping)
        {
            return looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
        }

        std::array<AXVoice*, 2> voices;
        int channels;

        bool ready;
        bool playing;
        mutable bool started;
    };
} // namespace love
