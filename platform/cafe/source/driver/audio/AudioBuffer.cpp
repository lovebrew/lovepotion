#include "driver/audio/AudioBuffer.hpp"
#include "driver/audio/DigitalSoundMix.hpp"

namespace love
{
    AudioBuffer::AudioBuffer() : data(nullptr), nsamples(0), voices {}, ready(false)
    {}

    AudioBuffer::~AudioBuffer()
    {
        if (!this->ready)
            return;

        for (int index = 0; index < this->channels; index++)
            AXFreeVoice(this->voices[index]);
    }

    void AudioBuffer::initialize(int channels)
    {
        for (int index = 0; index < channels; index++)
        {
            this->voices[index] = AXAcquireVoice(0x1F, nullptr, nullptr);

            AXVoiceBegin(this->voices[index]);
            AXSetVoiceType(this->voices[index], AX_VOICE_TYPE_UNKNOWN);
            AXVoiceEnd(this->voices[index]);
        }

        if (channels == 2 && !this->voices.back())
            throw love::Exception("Stereo voices require 2 voices to be available.");

        this->channels = channels;
        this->ready    = true;
    }

    bool AudioBuffer::prepare()
    {
        if (!this->ready)
            return false;

        if (!this->data || !this->nsamples)
            return false;

        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);

            AXSetVoiceState(this->voices[index], AX_VOICE_STATE_STOPPED);

            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->voices[index], &offsets);

            offsets.currentOffset = 0;
            offsets.endOffset     = this->nsamples;
            offsets.loopOffset    = 0;
            offsets.data          = this->data;

            AXSetVoiceOffsets(this->voices[index], &offsets);
            AXSetVoiceCurrentOffset(this->voices[index], 0);

            AXVoiceEnd(this->voices[index]);
        }

        return true;
    }

    void AudioBuffer::setDeviceMix(int channels)
    {
        if (!this->ready)
            return;

        // clang-format off
        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);

            switch (channels)
            {
                case AX_VOICE_MONO:
                    AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_TV, 0,  MONO_MIX[index]);
                    AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_DRC, 0, MONO_MIX[index]);
                    break;
                case AX_VOICE_STEREO:
                    AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_TV, 0,  STEREO_MIX[index]);
                    AXSetVoiceDeviceMix(this->voices[index], AX_DEVICE_TYPE_DRC, 0, STEREO_MIX[index]);
                    break;
                default:
                    break;
            }

            AXVoiceEnd(this->voices[index]);
        }
        // clang-format on
    }

    void AudioBuffer::setState(AXVoiceState state)
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

    bool AudioBuffer::setFormat(AXVoiceFormat format)
    {
        if (!this->ready)
            return false;

        if (format == AX_VOICE_FORMAT_ADPCM)
            return false;

        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);

            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->voices[index], &offsets);
            offsets.dataType = format;

            AXSetVoiceOffsets(this->voices[index], &offsets);

            AXVoiceEnd(this->voices[index]);
        }

        return true;
    }

    bool AudioBuffer::isFinished() const
    {
        if (!this->ready)
            return false;

        AXVoiceBegin(this->voices[0]);

        bool done = this->voices[0]->state == AX_VOICE_STATE_STOPPED;

        AXVoiceEnd(this->voices[0]);

        return done == true;
    }

    void AudioBuffer::setVolume(float volume)
    {
        volume = std::clamp(volume, 0.0f, 1.0f);

        AXVoiceVeData ve {
            .volume = 0x8000,
            .delta  = 0,
        };

        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);
            AXSetVoiceVe(this->voices[index], &ve);
            AXVoiceEnd(this->voices[index]);
        }
    }

    bool AudioBuffer::setSampleRateConversionRatio(float ratio)
    {
        if (ratio == 0.0f)
            return false;

        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);

            auto result = AXSetVoiceSrcRatio(this->voices[index], ratio);
            AXSetVoiceSrcType(this->voices[index], AX_VOICE_SRC_TYPE_LINEAR);

            AXVoiceEnd(this->voices[index]);
        }

        return true;
    }

    size_t AudioBuffer::getSampleOffset() const
    {
        AXVoiceOffsets offsets {};

        AXVoiceBegin(this->voices[0]);
        AXGetVoiceOffsets(this->voices[0], &offsets);
        AXVoiceEnd(this->voices[0]);

        return offsets.currentOffset;
    }

    void AudioBuffer::setLooping(bool looping)
    {
        for (int index = 0; index < this->channels; index++)
        {
            AXVoiceBegin(this->voices[index]);

            AXVoiceOffsets offsets {};
            AXGetVoiceOffsets(this->voices[index], &offsets);

            offsets.loopingEnabled = AudioBuffer::getLooping(looping);

            AXSetVoiceOffsets(this->voices[index], &offsets);

            AXVoiceEnd(this->voices[index]);
        }
    }

    bool AudioBuffer::isLooping() const
    {
        AXVoiceOffsets offsets {};

        AXVoiceBegin(this->voices[0]);
        AXGetVoiceOffsets(this->voices[0], &offsets);
        AXVoiceEnd(this->voices[0]);

        return offsets.loopingEnabled == AX_VOICE_LOOP_ENABLED;
    }
} // namespace love
