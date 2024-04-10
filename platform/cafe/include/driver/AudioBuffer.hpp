#pragma once

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
        AudioBuffer() : voices {}, channels(0), volume(1.0f), ready(false)
        {}

        ~AudioBuffer()
        {
            if (!this->ready)
                return;

            for (int index = 0; index < this->channels; index++)
                AXFreeVoice(this->voices[index]);
        }

        void initialize(int channels, AXVoiceFormat format)
        {
            this->channels = channels;

            for (int index = 0; index < channels; index++)
            {
                this->voices[index] = AXAcquireVoice(0x1F, nullptr, nullptr);

                if (!this->voices[index])
                    continue;

                AXVoiceBegin(this->voices[index]);
                AXSetVoiceType(this->voices[index], AX_VOICE_TYPE_UNKNOWN);
                AXVoiceEnd(this->voices[index]);
            }

            this->ready = true;
            this->setFormat(format);
        }

        bool isInitialized() const
        {
            return this->ready;
        }

        void prepare(size_t nsamples, void* data, size_t size, bool looping)
        {
            if (!this->ready)
                return;

            DCFlushRange(data, size);

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);

                AXSetVoiceState(this->voices[index], AX_VOICE_STATE_STOPPED);
                AXSetVoiceLoop(this->voices[index], AudioBuffer::getLooping(looping));

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->voices[index], &offsets);

                offsets.currentOffset = 0;
                offsets.endOffset     = nsamples;
                offsets.loopOffset    = 0;
                offsets.data          = data;

                AXSetVoiceOffsets(this->voices[index], &offsets);
                AXSetVoiceCurrentOffset(this->voices[index], 0);

                AXVoiceEnd(this->voices[index]);
            }
        }

        void setDeviceMix(AXVoiceDeviceMixData (&mix)[6])
        {
            if (!this->ready)
                return;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);
                AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_TV, 0, mix);
                AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_DRC, 0, mix);
                AXVoiceEnd(this->voices[index]);
            }
        }

        void setState(AXVoiceState state)
        {
            if (!this->ready)
                return;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);
                AXSetVoiceState(this->voices[index], state);
                AXVoiceEnd(this->voices[index]);
            }
        }

        void setFormat(AXVoiceFormat format)
        {
            std::printf("Trying format: %d\n", format);
            if (!this->ready)
                return;
            std::printf("Setting format: %d\n", format);
            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->voices[index], &offsets);
                offsets.dataType = format;

                AXSetVoiceOffsets(this->voices[index], &offsets);

                AXVoiceEnd(this->voices[index]);
            }
        }

        bool isFinished() const
        {
            if (!this->ready)
                return false;

            AXVoiceBegin(this->voices[0]);
            bool done = this->voices[0]->state == AX_VOICE_STATE_STOPPED;
            AXVoiceEnd(this->voices[0]);

            return done == true;
        }

        void setVolume(float volume)
        {
            volume = std::clamp(volume, 0.0f, 1.0f);

            AXVoiceVeData ve {};
            ve.volume = volume * 0x8000;

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);
                AXSetVoiceVe(this->voices[index], &ve);
                AXVoiceEnd(this->voices[index]);
            }

            this->volume = volume;
        }

        float getVolume() const
        {
            return this->volume;
        }

        bool setSampleRate(int sampleRate)
        {
            float ratio = (float)sampleRate / (float)AXGetInputSamplesPerSec();

            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);

                // clang-format off
                if (AXSetVoiceSrcRatio(this->voices[index], ratio) != AX_VOICE_RATIO_RESULT_SUCCESS)
                    return false;
                // clang-format on

                AXSetVoiceSrcType(this->voices[index], AX_VOICE_SRC_TYPE_LINEAR);

                AXVoiceEnd(this->voices[index]);
            }

            return true;
        }

        size_t getSampleOffset() const
        {
            AXVoiceOffsets offsets {};

            AXVoiceBegin(this->voices[0]);
            AXGetVoiceOffsets(this->voices[0], &offsets);
            AXVoiceEnd(this->voices[0]);

            return offsets.currentOffset;
        }

        void setLooping(bool looping)
        {
            for (int index = 0; index < this->channels; index++)
            {
                AXVoiceBegin(this->voices[index]);

                AXVoiceOffsets offsets {};
                AXGetVoiceOffsets(this->voices[index], &offsets);

                offsets.loopingEnabled = looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;

                AXSetVoiceOffsets(this->voices[index], &offsets);

                AXVoiceEnd(this->voices[index]);
            }
        }

      private:
        static AXVoiceLoop getLooping(bool looping)
        {
            return looping ? AX_VOICE_LOOP_ENABLED : AX_VOICE_LOOP_DISABLED;
        }

        std::array<AXVoice*, 2> voices;
        int channels;
        float volume;
        bool ready;
    };
} // namespace love
