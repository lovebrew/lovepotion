#pragma once

#include "driver/DigitalSound.tcc"
#include "driver/DigitalSoundMix.hpp"

#include <coreinit/event.h>

#include <sndcore2/core.h>
#include <sndcore2/device.h>
#include <sndcore2/voice.h>

#include <queue>

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

        struct Channel
        {
          public:
            enum ChannelState
            {
                STATE_PLAYING,
                STATE_PAUSED,
                STATE_STOPPED,
                STATE_DONE
            };

            Channel() : buffers {}, state(STATE_DONE), channels(0), volume(1.0f)
            {}

            void begin(AXVoice* voice, int channels)
            {
                for (int index = 0; index < channels; index++)
                    this->buffers[index] = voice;
            }

            bool reset(int channels, int bitDepth, int sampleRate, float volume)
            {
                volume = std::clamp(volume, 0.0f, 1.0f);

                AXVoiceVeData ve {};
                ve.volume = volume * 0x8000;

                int32_t format = 0;
                if (DigitalSound::getFormat(channels, bitDepth) < 0)
                    return false;

                for (int index = 0; index < channels; index++)
                {
                    if (this->buffers[index] == nullptr)
                        continue;

                    AXVoiceBegin(this->buffers[index]);
                    AXSetVoiceType(this->buffers[index], AX_VOICE_TYPE_UNKNOWN);
                    AXSetVoiceVe(this->buffers[index], &ve);

                    // clang-format off
                    switch (channels)
                    {
                        case AX_VOICE_MONO:
                        {
                            AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_DRC, 0, MONO_MIX[index]);
                            AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_TV,  0, MONO_MIX[index]);
                            break;
                        }
                        case AX_VOICE_STEREO:
                        {
                            AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_DRC, 0, STEREO_MIX[index]);
                            AXSetVoiceDeviceMix(this->buffers[index], AX_DEVICE_TYPE_TV,  0, STEREO_MIX[index]);
                            break;
                        }
                        default:
                            break;
                    }
                    // clang-format on

                    float ratio = (float)sampleRate / (float)AXGetInputSamplesPerSec();
                    if (auto result = AXSetVoiceSrcRatio(this->buffers[index], ratio); result != 0)
                        return false;

                    AXSetVoiceCurrentOffset(this->buffers[index], 0);
                    AXSetVoiceSrcType(this->buffers[index], AX_VOICE_SRC_TYPE_LINEAR);

                    AXVoiceOffsets offsets {};
                    AXGetVoiceOffsets(this->buffers[index], &offsets);

                    offsets.dataType = (AXVoiceFormat)format;
                    AXSetVoiceOffsets(this->buffers[index], &offsets);

                    AXVoiceEnd(this->buffers[index]);
                }

                this->channels = channels;
                return true;
            }

            void setVolume(float volume)
            {
                volume = std::clamp(volume, 0.0f, 1.0f);

                AXVoiceVeData ve {};
                ve.volume = volume * 0x8000;

                for (int index = 0; index < channels; index++)
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
                    // AXSetVoiceOffsets(this->buffers[index], &this->offsets);
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
            std::array<AudioBuffer*, 2> buffers;
            ChannelState state;

            int channels;
            float volume;
        };

        std::array<Channel, 24> channels;
        bool shutdown = false;
    };
} // namespace love
