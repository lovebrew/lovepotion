#pragma once

#include "modules/audio/RecordingDevice.tcc"

#include <mic/mic.h>

namespace love
{
    class RecordingDevice final : public RecordingDeviceBase
    {
      public:
        RecordingDevice(const char* name);

        ~RecordingDevice() override;

        bool start(int samples, int sampleRate, int bitDepth, int channels) override;

        void stop() override;

        SoundData* getData() override;

        const char* getName() const override;

        int getSampleCount() const override;

        int getMaxSamples() const override;

        int getSampleRate() const override;

        int getBitDepth() const override;

        int getChannelCount() const override;

        bool isRecording() const override;

        void setGain(uint8_t gain) override;

        uint8_t getGain() const override;

      private:
        int samples    = DEFAULT_SAMPLES;
        int sampleRate = DEFAULT_SAMPLE_RATE;
        int bitDepth   = DEFAULT_BIT_DEPTH;
        int channels   = DEFAULT_CHANNELS;
        std::string name;

        MICWorkMemory memory;
    };
} // namespace love
