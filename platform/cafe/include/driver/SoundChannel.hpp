#pragma once

#include <array>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include "driver/DigitalSoundMix.hpp"

#include <algorithm>
#include <array>

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

        SoundChannel() : buffers {}, state(STATE_DONE), channels(0), volume(1.0f)
        {}

        void begin(AXVoice* voice, int channels)
        {
            for (int index = 0; index < channels; index++)
                this->buffers[index] = voice;

            this->channels = channels;
        }

        void setFormat(AXVoiceFormat format)
        {
            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->buffers[index], &offsets);
                offsets.dataType = format;

                AXSetVoiceOffsets(this->buffers[index], &offsets);

                AXVoiceEnd(this->buffers[index]);
            }
        }

        bool reset(int channels, int sampleRate, float volume)
        {
            volume = std::clamp(volume, 0.0f, 1.0f);

            AXVoiceVeData ve {};
            ve.volume = volume * 0x8000;

            float ratio = (float)sampleRate / (float)AXGetInputSamplesPerSec();

            for (int index = 0; index < channels; index++)
            {
                if (this->buffers[index] == nullptr)
                    continue;

                AXVoiceBegin(this->buffers[index]);

                AXSetVoiceType(this->buffers[index], AX_VOICE_TYPE_UNKNOWN);
                AXSetVoiceVe(this->buffers[index], &ve);

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

                AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_TV, 0, DEVICE_MIX);
                AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_DRC, 0, DEVICE_MIX);

                // clang-format off
                if (AXSetVoiceSrcRatio(this->buffers[index], ratio) != AX_VOICE_RATIO_RESULT_SUCCESS)
                    return false;
                // clang-format on

                AXSetVoiceSrcType(this->buffers[index], AX_VOICE_SRC_TYPE_LINEAR);
                AXSetVoiceCurrentOffset(this->buffers[index], 0);

                AXVoiceEnd(this->buffers[index]);
            }

            return true;
        }

        void setVolume(float volume)
        {
            volume = std::clamp(volume, 0.0f, 1.0f);

            AXVoiceVeData ve {};
            ve.volume = volume * 0x8000;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);
                AXSetVoiceVe(this->buffers[index], &ve);
                AXVoiceEnd(this->buffers[index]);
            }

            this->volume = volume;
        }

        float getVolume() const
        {
            return this->volume;
        }

        void setLooping(bool looping)
        {
            auto voiceLooping = looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);
                AXSetVoiceLoop(this->buffers[index], voiceLooping);
                AXVoiceEnd(this->buffers[index]);
            }
        }

        bool play()
        {
            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);
                AXSetVoiceState(this->buffers[index], AX_VOICE_STATE_PLAYING);
                AXVoiceEnd(this->buffers[index]);
            }

            this->state = STATE_PLAYING;
            return true;
        }

        bool isPlaying() const
        {
            return this->state == STATE_PLAYING;
        }

        void stop()
        {
            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);
                AXSetVoiceState(this->buffers[index], AX_VOICE_STATE_STOPPED);
                AXVoiceEnd(this->buffers[index]);
            }

            this->state = STATE_STOPPED;
        }

        void setPaused(bool paused)
        {
            auto voiceState = paused ? AX_VOICE_STATE_STOPPED : AX_VOICE_STATE_PLAYING;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->buffers[index]);
                AXSetVoiceState(this->buffers[index], voiceState);
                AXVoiceEnd(this->buffers[index]);
            }

            this->state = paused ? STATE_PAUSED : STATE_PLAYING;
        }

        bool isPaused() const
        {
            return this->state == STATE_PAUSED;
        }

        size_t getSampleOffset() const
        {
            AXVoiceOffsets offsets {};

            AXVoiceBegin(this->buffers[0]);
            AXGetVoiceOffsets(this->buffers[0], &offsets);
            AXVoiceEnd(this->buffers[0]);

            return offsets.currentOffset;
        }

      private:
        std::array<AXVoice*, 2> buffers;
        ChannelState state;

        int channels;
        float volume;
    };
} // namespace love
