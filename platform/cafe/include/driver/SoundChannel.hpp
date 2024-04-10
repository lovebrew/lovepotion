#pragma once

#include <array>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include "driver/DigitalSoundMix.hpp"

#include <algorithm>
#include <queue>

namespace love
{
    class SoundChannel
    {
      public:
        enum ChannelState
        {
            STATE_PLAYING,
            STATE_PAUSED,
            STATE_STOPPED,
            STATE_DONE
        };

        SoundChannel() : buffers {}, state(STATE_DONE), volume(1.0f)
        {}

        bool reset(AudioBuffer* buffer, int channels, float volume)
        {
            if (!buffer->isInitialized())
                return false;

            this->current = buffer;

            volume = std::clamp(volume, 0.0f, 1.0f);

            switch (channels)
            {
                case AX_VOICE_MONO:
                    DEVICE_MIX[0].bus[0].volume = 0x8000;
                    break;
                case AX_VOICE_STEREO:
                    DEVICE_MIX[0].bus[0].volume = 0x8000;
                    DEVICE_MIX[1].bus[0].volume = 0x8000;
                    break;
                default:
                    return false;
            }

            this->current->setDeviceMix(DEVICE_MIX);
            this->current->setVolume(volume);

            return true;
        }

        void setVolume(float volume)
        {
            volume = std::clamp(volume, 0.0f, 1.0f);

            this->current->setVolume(volume);
            this->volume = volume;
        }

        float getVolume() const
        {
            return this->volume;
        }

        void setLooping(bool looping)
        {
            this->current->setLooping(looping);
        }

        bool play()
        {
            this->current->setState(AX_VOICE_STATE_PLAYING);
            this->state = STATE_PLAYING;

            return true;
        }

        bool isPlaying() const
        {
            return this->state == STATE_PLAYING;
        }

        void stop()
        {
            this->current->setState(AX_VOICE_STATE_STOPPED);
            this->state = STATE_STOPPED;
        }

        void setPaused(bool paused)
        {
            auto voiceState = paused ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;
            this->current->setState(voiceState);

            this->state = paused ? STATE_PAUSED : STATE_PLAYING;
        }

        bool isPaused() const
        {
            return this->state == STATE_PAUSED;
        }

        size_t getSampleOffset() const
        {
            return this->current->getSampleOffset();
        }

      private:
        std::queue<AudioBuffer*> buffers;
        AudioBuffer* current;

        ChannelState state;

        float volume;
    };
} // namespace love
